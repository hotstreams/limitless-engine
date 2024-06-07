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

void CascadeShadows::initBuffers(Context& context) {
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
          .build(DIRECTIONAL_CSM_BUFFER_NAME, context);
}

CascadeShadows::CascadeShadows(Context& context, const RendererSettings& settings)
    : shadow_resolution {settings.csm_resolution}
    , split_count {settings.csm_split_count} {
    initBuffers(context);
    frustums.resize(split_count);
    far_bounds.resize(split_count);
    light_space.reserve(split_count);
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

        for (uint32_t i = 1; i < split_count; ++i) {
            const auto si = static_cast<float>(i) / static_cast<float>(split_count);
            frustums[i].near_distance = SPLIT_WEIGHT * (near * glm::pow(ratio, si)) + (1 - SPLIT_WEIGHT) * (near + (far - near) * si);
            frustums[i - 1].far_distance = frustums[i].near_distance * 1.005f;
        }

        frustums[split_count - 1].far_distance = far;
    }

    // updates far distances for uniform
    {
        const auto projection = camera.getProjection();
        for (uint32_t i = 0; i < split_count; ++i) {
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

void CascadeShadows::updateLightMatrices(const Light& light) {
    // clear matrices
    light_space.clear();

    auto up = glm::vec3{0.0f, 1.0f, 0.0f};

    if (glm::abs(glm::dot(up, light.getDirection())) > 0.999f) {
        up = {0.0f, 0.0f, 1.0f};
    }

    const auto view = glm::lookAt(-light.getDirection(), { 0.0f, 0.0f, 0.0f }, up);
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
        light_space.emplace_back(frustum.crop);
    }
}

void CascadeShadows::draw(Instances& instances,
                          const Light& light,
                          Context& ctx, const
                          Assets& assets,
                          const Camera& camera,
                          [[maybe_unused]] fx::EffectRenderer* renderer) {
    updateFrustums(ctx, camera);
    updateLightMatrices(light);

    framebuffer->bind();

    ctx.setViewPort(shadow_resolution);
    ctx.setDepthMask(DepthMask::True);
    ctx.setDepthFunc(DepthFunc::Less);
    ctx.enable(Capabilities::DepthTest);

    for (uint32_t i = 0; i < split_count; ++i) {
        framebuffer->specifyLayer(FramebufferAttachment::Depth, i);
        framebuffer->clear();

        const auto uniform_set = [&] (ShaderProgram& shader) {
            shader.setUniform("light_space", frustums[i].crop);
        };

        for (const auto& instance : instances) {
            if (!instance.get().doesCastShadow()) {
                continue;
            }

            if (instance.get().getInstanceType() != InstanceType::Decal) {
                instance.get().draw(ctx, assets, ShaderType::DirectionalShadow, ms::Blending::Opaque, UniformSetter{uniform_set});
            }
        }

//        if (renderer) {
//            renderer->draw(ctx, assets, ShaderPass::DirectionalShadow, ms::Blending::Opaque, UniformSetter{uniform_set});
//        }
    }

    framebuffer->unbind();
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

void CascadeShadows::update(Context& ctx, const RendererSettings& settings) {
    shadow_resolution = settings.csm_resolution;
    split_count = settings.csm_split_count;

    initBuffers(ctx);

    frustums.resize(split_count);
    far_bounds.resize(split_count);
}

CascadeShadows::~CascadeShadows() {
    if (auto* ctx = Context::getCurrentContext(); ctx) {
        ctx->getIndexedBuffers().remove(DIRECTIONAL_CSM_BUFFER_NAME, light_buffer);
    }
}
