#include <limitless/lighting/cascade_shadows.hpp>

#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include "limitless/core/uniform/uniform_setter.hpp"
#include "limitless/core/uniform/uniform.hpp"

#include <limitless/ms/blending.hpp>
#include <limitless/instances/instance.hpp>

#include <limitless/lighting/light.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/camera.hpp>
#include <limitless/scene.hpp>

#include <limitless/fx/effect_renderer.hpp>
#include <iostream>

using namespace Limitless;

namespace {
    constexpr auto DIRECTIONAL_CSM_BUFFER_NAME = "directional_shadows";
}

// Implementation of CascadeSplits class
CascadeShadows::CascadeSplits::CascadeSplits(const Params& params) {
    splitCount = params.cascadeCount + 1;
    splits.resize(splitCount);
    
    // First split is always at near plane
    splits[0] = params.near;
    
    // Last split is always at far plane
    splits[params.cascadeCount] = params.far;
    
    // Copy intermediate split positions
    for (size_t i = 1; i < params.cascadeCount; i++) {
        // Convert from normalized [0,1] position to absolute depth
        splits[i] = params.near + (params.far - params.near) * params.splitPositions[i - 1];
    }
}

void CascadeShadows::CascadeSplits::computeUniformSplits(float* splitPositions, uint8_t cascades) {
    for (uint8_t i = 1; i < cascades; i++) {
        splitPositions[i - 1] = static_cast<float>(i) / static_cast<float>(cascades);
    }
}

void CascadeShadows::CascadeSplits::computeLogSplits(float* splitPositions, uint8_t cascades,
        float near, float far) {
    for (uint8_t i = 1; i < cascades; i++) {
        splitPositions[i - 1] = 
            (near * std::pow(far / near, static_cast<float>(i) / static_cast<float>(cascades)) - near) / (far - near);
    }
}

void CascadeShadows::CascadeSplits::computePracticalSplits(float* splitPositions, uint8_t cascades,
        float near, float far, float lambda) {
    // Calculate both uniform and logarithmic splits
    std::vector<float> uniformSplits(cascades - 1);
    std::vector<float> logSplits(cascades - 1);
    
    computeUniformSplits(uniformSplits.data(), cascades);
    computeLogSplits(logSplits.data(), cascades, near, far);
    
    // Interpolate between uniform and logarithmic split schemes
    for (uint8_t i = 1; i < cascades; i++) {
        splitPositions[i - 1] = lambda * logSplits[i - 1] + (1.0f - lambda) * uniformSplits[i - 1];
    }
}

void CascadeShadows::initBuffers() {
    auto depth = Texture::builder()
            .target(Texture::Type::Tex2DArray)
            .internal_format(Texture::InternalFormat::Depth16)
            .size(glm::uvec3{shadow_resolution, split_count})
            .format(Texture::Format::DepthComponent)
            .data_type(Texture::DataType::Float)
            .mipmap(false)
            .levels(1)
            .min_filter(Texture::Filter::Nearest)
            .mag_filter(Texture::Filter::Nearest)
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .wrap_r(Texture::Wrap::ClampToEdge)
            .build();

    framebuffer = std::make_unique<Framebuffer>();
    framebuffer->bind();
    *framebuffer << TextureAttachment{FramebufferAttachment::Depth, depth};
    framebuffer->specifyLayer(FramebufferAttachment::Depth, 0);
    framebuffer->drawBuffer(FramebufferAttachment::None);
    framebuffer->readBuffer(FramebufferAttachment::None);
    framebuffer->checkStatus();
    framebuffer->unbind();

    light_buffer = Buffer::builder()
          .target(Buffer::Type::ShaderStorage)
          .usage(Buffer::Usage::DynamicDraw)
          .access(Buffer::MutableAccess::WriteOrphaning)
          .size(sizeof(glm::mat4) * split_count)
          .build(DIRECTIONAL_CSM_BUFFER_NAME, *Context::getCurrentContext());
}

CascadeShadows::CascadeShadows(const RendererSettings& settings)
    : shadow_resolution {settings.csm_resolution}
    , split_count {settings.csm_split_count} {
    initBuffers();
    frustums.resize(split_count);
    far_bounds.resize(split_count);
    light_space.reserve(split_count);
}

// Calculate near/far planes in view space
glm::vec2 CascadeShadows::computeNearFar(const glm::mat4& view, const glm::vec3* vertices, size_t count) {
    glm::vec2 nearFar = { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() };
    
    for (size_t i = 0; i < count; i++) {
        // Transform vertex to view space and get z coordinate (depth)
        glm::vec4 viewPos = view * glm::vec4(vertices[i], 1.0f);
        float depth = viewPos.z;
        
        // Update near/far planes
        nearFar.x = std::max(nearFar.x, depth);  // Near plane (max depth value)
        nearFar.y = std::min(nearFar.y, depth);  // Far plane (min depth value)
    }
    
    return nearFar;
}

// Calculate near/far planes in warp space
glm::vec2 CascadeShadows::computeNearFarOfWarpSpace(const glm::mat4& lightView, const glm::vec3* vertices, size_t count) {
    glm::vec2 nearFar = { std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest() };
    
    for (size_t i = 0; i < count; i++) {
        // Transform vertex to light view space and get y coordinate (LiSPSM uses y-axis)
        glm::vec4 viewPos = lightView * glm::vec4(vertices[i], 1.0f);
        float y = viewPos.y;
        
        // Update near/far in warp space
        nearFar.x = std::min(nearFar.x, y);  // Near (min y value)
        nearFar.y = std::max(nearFar.y, y);  // Far (max y value)
    }
    
    return nearFar;
}

// Calculate 2D bounds of points projected into light space
AABB CascadeShadows::compute2DBounds(const glm::mat4& lightView, const glm::vec3* vertices, size_t count) {
    AABB bounds;
    bounds.min = glm::vec3(std::numeric_limits<float>::max());
    bounds.max = glm::vec3(std::numeric_limits<float>::lowest());
    
    for (size_t i = 0; i < count; i++) {
        // Project vertex to light space
        glm::vec4 lightPos = lightView * glm::vec4(vertices[i], 1.0f);
        glm::vec3 projected = glm::vec3(lightPos) / lightPos.w;
        
        // Update XY bounds
        bounds.min.x = std::min(bounds.min.x, projected.x);
        bounds.min.y = std::min(bounds.min.y, projected.y);
        bounds.max.x = std::max(bounds.max.x, projected.x);
        bounds.max.y = std::max(bounds.max.y, projected.y);
    }
    
    return bounds;
}

// Update scene info for shadow calculation
void CascadeShadows::updateSceneInfo(const Scene& scene) {
    // Reset scene info
    sceneInfo.shadowCastersVolume = AABB();
    sceneInfo.shadowReceiversVolume = AABB();
    
    // TODO: Collect all shadow casters and receivers from the scene
    // This needs to be implemented based on your scene data structure
    // For now, we'll use a simple approximation
    
    // For demo purposes - create a simple default box if no objects were found
    if (sceneInfo.shadowCastersVolume.isEmpty()) {
        sceneInfo.shadowCastersVolume.min = glm::vec3(-50.0f, -50.0f, -50.0f);
        sceneInfo.shadowCastersVolume.max = glm::vec3(50.0f, 50.0f, 50.0f);
    }
    
    if (sceneInfo.shadowReceiversVolume.isEmpty()) {
        sceneInfo.shadowReceiversVolume = sceneInfo.shadowCastersVolume;
    }
}

// Apply LiSPSM warping for better shadow quality
glm::mat4 CascadeShadows::applyLISPSM(glm::mat4& Wp, 
                                     const Camera& camera,
                                     const glm::mat4& LMp,
                                     const glm::mat4& Mv,
                                     const glm::mat4& LMpMv,
                                     const FrustumBoxIntersection& shadowVolume,
                                     size_t vertexCount,
                                     const glm::vec3& dir) {
    // Calculate dot product between camera direction and light direction
    const float LoV = glm::dot(camera.getDirection(), dir);
    const float sinLV = std::sqrt(std::max(0.0f, 1.0f - LoV * LoV));
    
    // Skip LiSPSM if camera direction and light direction are too close
    if (std::abs(LoV) > 0.99f) {
        return glm::mat4(1.0f);
    }
    
    // Calculate near/far in warp space
    const glm::vec2 nf = computeNearFarOfWarpSpace(LMp, shadowVolume.data(), vertexCount);
    const float n = nf.x;  // Near plane in warp space
    const float f = nf.y;  // Far plane in warp space
    const float d = std::abs(f - n);  // Depth range
    
    // Calculate optimal near plane
    // This is a simplified version of the algorithm described in the LiSPSM paper
    const float near = camera.getNear();
    const float far = camera.getFar();
    
    const float z0 = near;
    const float z1 = std::min(far, 100.0f);  // Use a reasonable far value
    
    // Virtual near plane (similar to shadowNearHint in Filament)
    float vz0 = std::max(0.1f, z0);
    float vz1 = std::max(vz0 + 0.1f, z1);
    
    // Calculate optimal near plane distance
    const float nopt = (1.0f / sinLV) * (z0 + std::sqrt(vz0 * vz1));
    
    // Calculate LiSPSM warping matrix
    // First translate to position, then apply warp
    glm::vec3 lsCameraPosition = glm::vec3(LMpMv * glm::vec4(camera.getPosition(), 1.0f));
    glm::vec3 p(lsCameraPosition.x, n, lsCameraPosition.z);
    
    // Apply translation and warping
    const glm::mat4 Wv = glm::translate(glm::mat4(1.0f), -p);
    Wp = warpFrustum(nopt, nopt + d);
    
    return Wp * Wv;
}

// Update frustums for cascaded shadow mapping
void CascadeShadows::updateFrustums(const Camera& camera) {
    // Get camera parameters
    const float far = camera.getFar();
    const float near = camera.getNear();
    const float fov = glm::radians(camera.getFov());
    const float aspect = camera.getAspect();
    
    // Create array for split positions
    std::array<float, CascadeSplits::MAX_SPLITS> splitPositions{};
    splitPositions[split_count] = 1.0f;
    
    // Use practical split scheme with configurable lambda
    CascadeSplits::computePracticalSplits(splitPositions.data(), split_count, near, far, SPLIT_WEIGHT);
    
    // Create cascade splits
    CascadeSplits::Params params;
    params.near = near;
    params.far = far;
    params.cascadeCount = split_count;
    params.splitPositions = splitPositions;
    
    CascadeSplits splits(params);
    
    // For each cascade, calculate frustum parameters
    for (uint32_t i = 0; i < split_count; ++i) {
        ShadowFrustum& frustum = frustums[i];
        
        frustum.fov = fov;
        frustum.ratio = aspect;
        frustum.near_distance = -*splits.begin() + i;
        frustum.far_distance = -*splits.begin() + i + 1;
        
        // Calculate frustum corners
        // This is a simplification - in a real implementation, we would use a proper frustum extraction
        float nearHeight = 2.0f * std::tan(fov * 0.5f) * std::abs(frustum.near_distance);
        float nearWidth = nearHeight * aspect;
        float farHeight = 2.0f * std::tan(fov * 0.5f) * std::abs(frustum.far_distance);
        float farWidth = farHeight * aspect;
        
        // Calculate frustum corners
        // Near plane corners
        frustum.points[0] = glm::vec3(-nearWidth * 0.5f,  nearHeight * 0.5f, frustum.near_distance);
        frustum.points[1] = glm::vec3( nearWidth * 0.5f,  nearHeight * 0.5f, frustum.near_distance);
        frustum.points[2] = glm::vec3( nearWidth * 0.5f, -nearHeight * 0.5f, frustum.near_distance);
        frustum.points[3] = glm::vec3(-nearWidth * 0.5f, -nearHeight * 0.5f, frustum.near_distance);
        
        // Far plane corners
        frustum.points[4] = glm::vec3(-farWidth * 0.5f,  farHeight * 0.5f, frustum.far_distance);
        frustum.points[5] = glm::vec3( farWidth * 0.5f,  farHeight * 0.5f, frustum.far_distance);
        frustum.points[6] = glm::vec3( farWidth * 0.5f, -farHeight * 0.5f, frustum.far_distance);
        frustum.points[7] = glm::vec3(-farWidth * 0.5f, -farHeight * 0.5f, frustum.far_distance);
        
        // Transform frustum corners from view space to world space
        const glm::mat4 invView = glm::inverse(camera.getView());
        for (int j = 0; j < 8; j++) {
            frustum.points[j] = glm::vec3(invView * glm::vec4(frustum.points[j], 1.0f));
        }
    }
    
    // Update shadow bounds based on the projection matrix
    const auto& projection = camera.getProjection();
    for (uint32_t i = 0; i < split_count; ++i) {
        far_bounds[i] = 0.5f * (-frustums[i].far_distance * projection[2][2] + projection[3][2]) / frustums[i].far_distance + 0.5f;
    }
}

// Update light matrices with Filament-style CSM
void CascadeShadows::updateLightMatrices(const Light& light, const Camera& camera) {
    // Clear previous light space matrices
    light_space.clear();
    
    // Get light direction and create a suitable up vector
    glm::vec3 lightDir = -light.getDirection();  // Light direction points from the source
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    
    // Adjust up vector if too close to light direction
    if (std::abs(glm::dot(up, lightDir)) > 0.99f) {
        up = glm::vec3(1.0f, 0.0f, 0.0f);
    }
    
    // Calculate view matrix for directional light
    const glm::mat4 lightView = getDirectionalLightViewMatrix(lightDir, up);
    
    // Process each cascade
    for (uint32_t i = 0; i < split_count; ++i) {
        const auto& frustum = frustums[i];
        
        // Calculate shadow bounds in light space
        glm::vec3 min(std::numeric_limits<float>::max());
        glm::vec3 max(std::numeric_limits<float>::lowest());
        
        // Find min/max in light space for this frustum
        for (const auto& point : frustum.points) {
            glm::vec4 lightSpacePoint = lightView * glm::vec4(point, 1.0f);
            
            min.x = std::min(min.x, lightSpacePoint.x);
            min.y = std::min(min.y, lightSpacePoint.y);
            min.z = std::min(min.z, lightSpacePoint.z);
            
            max.x = std::max(max.x, lightSpacePoint.x);
            max.y = std::max(max.y, lightSpacePoint.y);
            max.z = std::max(max.z, lightSpacePoint.z);
        }
        
        // Add extra depth range for shadow casters that might be outside the view frustum
        min.z -= 10.0f;  // Move near plane back
        
        // Stabilize shadows by aligning to texels if enabled
        if (shadowOptions.stable) {
            // Texel size in world units
            float texelSize = (max.x - min.x) / shadow_resolution.x;
            
            // Align bounds to texel grid
            min.x = std::floor(min.x / texelSize) * texelSize;
            min.y = std::floor(min.y / texelSize) * texelSize;
            max.x = std::ceil(max.x / texelSize) * texelSize;
            max.y = std::ceil(max.y / texelSize) * texelSize;
        }
        
        // Create orthographic projection for this cascade
        const glm::mat4 lightProjection = glm::ortho(
            min.x, max.x,
            min.y, max.y,
            min.z, max.z
        );
        
        // Store the final light space transformation matrix
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        
        // Apply LiSPSM warping if enabled and if not in stable mode
        if (shadowOptions.lispsm && !shadowOptions.stable) {
            // Create dummy shadow volume for LiSPSM
            FrustumBoxIntersection shadowVolume(frustum.points, frustum.points + 8);
            
            // Apply LiSPSM warping
            glm::mat4 Wp(1.0f);
            glm::mat4 warpMatrix = applyLISPSM(
                Wp, camera, 
                lightProjection, lightView, 
                lightSpaceMatrix, shadowVolume, 8,
                lightDir
            );
            
            // Apply warping to final matrix if valid
            if (warpMatrix != glm::mat4(1.0f)) {
                lightSpaceMatrix = warpMatrix * lightSpaceMatrix;
            }
        }
        
        // Store the shadow matrix for this cascade
        light_space.push_back(lightSpaceMatrix);
    }
}

// Prepare shadow maps for rendering
void CascadeShadows::prepare(const Scene& scene, const Camera& camera) {
    // Update scene information for shadow calculations
    updateSceneInfo(scene);
    
    // Update frustums based on camera
    updateFrustums(camera);
    
    // Update light matrices for shadow mapping
    updateLightMatrices(scene.getLighting().getDirectionalLight(), camera);
}

// Draw shadows for each cascade
void CascadeShadows::draw(InstanceRenderer& renderer, Scene& scene, Context& ctx, const Assets& assets, const Camera& camera) {
    // Prepare shadow mapping data
    prepare(scene, camera);
    
    // Bind framebuffer for shadow rendering
    framebuffer->bind();
    
    // Setup GL state for shadow mapping
    ctx.setViewPort(shadow_resolution);
    ctx.setDepthMask(DepthMask::True);
    ctx.setDepthFunc(DepthFunc::Less);
    ctx.enable(Capabilities::DepthTest);
    
    // Draw each cascade
    for (uint32_t i = 0; i < split_count; ++i) {
        // Select the appropriate layer in the depth texture array
        framebuffer->specifyLayer(FramebufferAttachment::Depth, i);
        framebuffer->clear();
        
        // Setup uniform for this cascade
        const auto uniform_set = [&](ShaderProgram& shader) {
            shader.setUniform("light_space", light_space[i]);
        };
        
        // Render shadow casters
        renderer.renderScene({ctx, assets, ShaderType::DirectionalShadow, ms::Blending::Opaque, UniformSetter{uniform_set}});
    }
    
    // Restore framebuffer
    framebuffer->unbind();
    
    // Update shadow map data in GPU buffer
    mapData();
}

void CascadeShadows::setUniform(ShaderProgram& shader) const {
	if (auto* ctx = Context::getCurrentContext(); ctx) {
		light_buffer->bindBase(ctx->getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, DIRECTIONAL_CSM_BUFFER_NAME));
	}

    shader.setUniform("_dir_shadows", framebuffer->get(FramebufferAttachment::Depth).texture);

    // TODO: ?
    glm::vec4 bounds {0.0f};
    for (uint32_t i = 0; i < split_count; ++i) {
        bounds[i] = far_bounds[i];
    }

    shader.setUniform("_far_bounds", bounds);
}

void CascadeShadows::mapData() const {
    light_buffer->mapData(light_space.data(), light_space.size() * sizeof(glm::mat4));
}

void CascadeShadows::update(const RendererSettings& settings) {
    shadow_resolution = settings.csm_resolution;
    split_count = settings.csm_split_count;

    initBuffers();

    frustums.resize(split_count);
    far_bounds.resize(split_count);
}

CascadeShadows::~CascadeShadows() {
    if (auto* ctx = Context::getCurrentContext(); ctx) {
        ctx->getIndexedBuffers().remove(DIRECTIONAL_CSM_BUFFER_NAME, light_buffer);
    }
}

// Implementation of directional light view matrix
glm::mat4 CascadeShadows::getDirectionalLightViewMatrix(glm::vec3 direction, glm::vec3 up, glm::vec3 position) {
    // Normalize direction for safety
    direction = glm::normalize(direction);
    
    // If direction and up vector are nearly parallel, use a different up vector
    if (std::abs(glm::dot(direction, up)) > 0.999f) {
        up = glm::vec3(1.0f, 0.0f, 0.0f);
        
        // If still parallel, try another direction
        if (std::abs(glm::dot(direction, up)) > 0.999f) {
            up = glm::vec3(0.0f, 0.0f, 1.0f);
        }
    }
    
    // Create look-at matrix for directional light
    // Position is used as the target, and position - direction is the eye position
    return glm::lookAt(position - direction, position, up);
}

// Implementation of directional light frustum matrix
glm::mat4 CascadeShadows::directionalLightFrustum(float near, float far) {
    // Create orthographic projection for directional light
    // Use same width/height (-1 to 1) with the specified near/far planes
    return glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near, far);
}

// Implementation of warp frustum for LiSPSM
glm::mat4 CascadeShadows::warpFrustum(float n, float f) {
    // Check that far is greater than near
    assert(f > n);
    
    // Calculate values for the perspective warp matrix
    const float d = 1.0f / (f - n);
    const float A = (f + n) * d;
    const float B = -2.0f * n * f * d;
    
    // Create warping matrix for LiSPSM - similar to a perspective projection
    // but looking down the Y axis with specific near/far values
    glm::mat4 m(1.0f);
    m[1][1] = A;
    m[1][3] = 1.0f;
    m[3][1] = B;
    m[3][3] = 0.0f;
    
    return m;
}
