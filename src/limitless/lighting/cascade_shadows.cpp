#include <limitless/lighting/cascade_shadows.hpp>

#include <limitless/core/texture_builder.hpp>
#include <limitless/core/buffer_builder.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/core/uniform_setter.hpp>
#include <limitless/core/uniform.hpp>

#include <limitless/ms/blending.hpp>
#include <limitless/instances/abstract_instance.hpp>

#include <limitless/lighting/lights.hpp>
#include <limitless/pipeline/renderer.hpp>
#include <limitless/camera.hpp>
#include <limitless/scene.hpp>

using namespace Limitless;

void CascadeShadows::initBuffers(Context& context) {
    TextureBuilder builder;
    auto depth = builder.setTarget(Texture::Type::Tex2DArray)
                        .setInternalFormat(Texture::InternalFormat::Depth16)
                        .setSize(glm::uvec3{SHADOW_RESOLUTION, SPLIT_COUNT})
                        .setFormat(Texture::Format::DepthComponent)
                        .setDataType(Texture::DataType::Float)
                        .setParameters([] (Texture& texture) {
                            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
                            texture << TexParameter<GLint>{GL_TEXTURE_MAG_FILTER, GL_LINEAR}
                                    << TexParameter<GLint>{GL_TEXTURE_MIN_FILTER, GL_LINEAR}
                                    << TexParameter<GLint>{GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER}
                                    << TexParameter<GLint>{GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER}
                                    << TexParameter<float*>{GL_TEXTURE_BORDER_COLOR, borderColor};
                        })
                        .build();

    fbo.bind();
    fbo << TextureAttachment{FramebufferAttachment::Depth, depth};
    fbo.specifyLayer(FramebufferAttachment::Depth, 0);
    fbo.drawBuffer(FramebufferAttachment::None);
    fbo.readBuffer(FramebufferAttachment::None);
    fbo.checkStatus();
    fbo.unbind();


    BufferBuilder buffer_builder;
    light_buffer = buffer_builder .setTarget(Buffer::Type::ShaderStorage)
                                  .setUsage(Buffer::Usage::DynamicDraw)
                                  .setAccess(Buffer::MutableAccess::WriteOrphaning)
                                  .setDataSize(sizeof(glm::mat4) * SPLIT_COUNT)
                                  .build("directional_shadows", context);
}

CascadeShadows::CascadeShadows(Context& context) {
    initBuffers(context);
    light_space.reserve(SPLIT_COUNT);
}

void CascadeShadows::updateFrustums(Context& ctx, const Camera& camera) {
    for (auto& frustum : frustums) {
        frustum.fov = glm::radians(camera.getFov());
        frustum.ratio = static_cast<float>(ctx.getSize().x) / static_cast<float>(ctx.getSize().y);
    }

    // updates split distances
    {
        const auto far = camera.getFar();
        const auto near = camera.getNear();
        const auto ratio = far / near;

        frustums[0].near_distance = near;

        for (uint32_t i = 1; i < SPLIT_COUNT; ++i) {
            const auto si = static_cast<float>(i) / static_cast<float>(SPLIT_COUNT);
            frustums[i].near_distance = SPLIT_WEIGHT * (near * glm::pow(ratio, si)) + (1 - SPLIT_WEIGHT) * (near + (far - near) * si);
            frustums[i - 1].far_distance = frustums[i].near_distance * 1.005f;
        }

        frustums[SPLIT_COUNT - 1].far_distance = far;
    }

    // updates far distances for uniform
    {
        const auto projection = camera.getProjection();
        for (uint32_t i = 0; i < SPLIT_COUNT; ++i) {
            far_bounds[i] = 0.5f * (-frustums[i].far_distance * projection[2][2] + projection[3][2]) / frustums[i].far_distance + 0.5f;
        }
    }

    // updates frustum points
    {
        const auto position = camera.getPosition();
        const auto front = camera.getFront();
        const auto right = camera.getRight();
        const auto up = camera.getUp();

        for (auto& frustum : frustums) {
            const auto fc = position + front * frustum.far_distance;
            const auto nc = position + front * frustum.near_distance;

            const auto near_height = glm::tan(frustum.fov / 2.0f) * frustum.near_distance;
            const auto far_height = glm::tan(frustum.fov / 2.0f) * frustum.far_distance;

            const auto near_width = near_height * frustum.ratio;
            const auto far_width = far_height * frustum.ratio;

            frustum.points[0] = nc - up * near_height - right * near_width;
            frustum.points[1] = nc + up * near_height - right * near_width;
            frustum.points[2] = nc + up * near_height + right * near_width;
            frustum.points[3] = nc - up * near_height + right * near_width;

            frustum.points[4] = fc - up * far_height - right * far_width;
            frustum.points[5] = fc + up * far_height - right * far_width;
            frustum.points[6] = fc + up * far_height + right * far_width;
            frustum.points[7] = fc - up * far_height + right * far_width;
        }
    }
}

void CascadeShadows::updateLightMatrices(const DirectionalLight& light) {
    // clear matrices
    light_space.clear();

    const auto view = glm::lookAt(-glm::vec3(light.direction), { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
    for (auto& frustum : frustums) {
        glm::vec3 max = {std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), 0.0f};
        glm::vec3 min = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), 0.0f};

        auto transform = view * glm::vec4(frustum.points[0], 1.0f);
        min.z = transform.z;
        max.z = transform.z;

        for (int j = 1; j < 8; j++) {
            transform = view * glm::vec4(frustum.points[j], 1.0f);
            max.z = glm::max(max.z, transform.z);
            min.z = glm::min(min.z, transform.z);
        }

        // TODO: make sure all relevant shadow casters are included, solve this shit, add AABB
        max.z += 50;

        const auto projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -max.z, -min.z);
        const auto mvp = projection * view;

        for (const auto& point : frustum.points) {
            transform = mvp * glm::vec4(point, 1.0f);

            transform.x /= transform.w;
            transform.y /= transform.w;

            max.x = glm::max(transform.x, max.x);
            max.y = glm::max(transform.y, max.y);
            min.x = glm::min(transform.x, min.x);
            min.y = glm::min(transform.y, min.y);
        }

        const auto scale = glm::vec2(2.0f / (max.x - min.x), 2.0f / (max.y - min.y));
        const auto offset = glm::vec2(-0.5f * (max.x + min.x) * scale.x, -0.5f * (max.y + min.y) * scale.y);

        glm::mat4 crop {1.0f};
        crop[0][0] = scale.x;
        crop[1][1] = scale.y;
        crop[0][3] = offset.x;
        crop[1][3] = offset.y;
        crop = glm::transpose(crop);

        frustum.crop = crop * projection * view;
        // save it for shader
        light_space.emplace_back(frustum.crop);
    }
}

void CascadeShadows::draw(Instances& instances, const DirectionalLight& light, Context& ctx, const Assets& assets, const Camera& camera) {
    updateFrustums(ctx, camera);
    updateLightMatrices(light);

    fbo.bind();

    ctx.setViewPort(SHADOW_RESOLUTION);
    ctx.setDepthMask(DepthMask::True);

    for (uint32_t i = 0; i < SPLIT_COUNT; ++i) {
        fbo.specifyLayer(FramebufferAttachment::Depth, i);
        fbo.clear();

        const auto uniform_set = [&] (ShaderProgram& shader) {
            shader << UniformValue{"light_space", frustums[i].crop};
        };

        for (const auto& instance : instances) {
            if (!instance.get().doesCastShadow()) {
                continue;
            }

            instance.get().draw(ctx, assets, ShaderPass::DirectionalShadow, ms::Blending::Opaque, UniformSetter{uniform_set});
        }
    }

    fbo.unbind();
}

void CascadeShadows::setUniform(ShaderProgram& shader)  const {
    shader << UniformSampler{"dir_shadows", fbo.get(FramebufferAttachment::Depth).texture};

    // TODO: ?
    glm::vec4 bounds {0.0f};
    for (uint32_t i = 0; i < SPLIT_COUNT; ++i) {
        bounds[i] = far_bounds[i];
    }

    shader << UniformValue{"far_bounds", bounds};
}

void CascadeShadows::mapData() const {
    light_buffer->mapData(light_space.data(), light_space.size() * sizeof(glm::mat4));
}