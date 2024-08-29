#include <limitless/renderer/color_picker.hpp>

#include <limitless/renderer/shader_type.hpp>
#include <utility>
#include <limitless/core/uniform/uniform_sampler.hpp>
#include <limitless/renderer/instance_renderer.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/renderer/depth_pass.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>

using namespace Limitless;

glm::vec3 ColorPicker::convert(uint32_t id) noexcept {
    const auto r = (id & 0x000000FF) >> 0;
    const auto g = (id & 0x0000FF00) >> 8;
    const auto b = (id & 0x00FF0000) >> 16;
    return glm::vec3{r, g, b} / 255.0f;
}

uint32_t ColorPicker::convert(glm::uvec3 color) noexcept {
    return color.r + color.g * 256 + color.b * 256 * 256;
}

ColorPicker::ColorPicker(Renderer& renderer)
    : RendererPass(renderer)
    , framebuffer {Framebuffer::asRGB8LinearClampToEdgeWithDepth(renderer.getResolution(), renderer.getPass<DeferredFramebufferPass>().getDepth())} {
}

void ColorPicker::onPick(Context& ctx, glm::uvec2 coords, std::function<void(uint32_t)> callback) {
    coords.y = ctx.getSize().y - coords.y;
    data.emplace_back(PickData{std::move(callback), coords});
}

void ColorPicker::process(Context& ctx) {
    if (!data.empty() && data.front().sync.isDone()) {
        const auto& info = data.front();

        framebuffer.bind();

        ctx.setPixelStore(PixelStore::UnpackAlignment, 1);

        glm::uvec3 color {};
        glReadPixels(info.coords.x, info.coords.y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &color[0]);

        framebuffer.unbind();

        info.callback(convert(color));

        data.pop_front();
    }
}

void ColorPicker::update([[maybe_unused]] Scene& scene, [[maybe_unused]] const Camera &camera) {
    auto& frustum_culling = renderer.getInstanceRenderer().getFrustumCulling();
    //TODO: buffer changed check?
    // remove old (removed from scene) buffers
    for (const auto& instance: frustum_culling.getVisibleInstances()) {
        if (instance->getInstanceType() == InstanceType::Instanced) {
            if (instanced_buffers.count(instance->getId()) == 0) {
                instanced_buffers[instance->getId()] = Buffer::builder()
                    .target(Buffer::Type::ShaderStorage)
                    .usage(Buffer::Usage::DynamicDraw)
                    .access(Buffer::MutableAccess::WriteOrphaning)
                    .data(nullptr)
                    .size(sizeof(glm::vec3))
                    .build("color_buffer", *Context::getCurrentContext());
            } else {
                auto instances = frustum_culling.getVisibleModelInstanced(static_cast<InstancedInstance&>(*instance));
                std::vector<glm::vec3> colors;
                colors.reserve(instances.size());
                for (const auto& inst: instances) {
                    colors.emplace_back(convert(inst->getId()));
                }
                // ensure buffer size
                auto& buffer = instanced_buffers[instance->getId()];
                auto size = sizeof(glm::vec3) * colors.size();
                if (buffer->getSize() < size) {
                    buffer->resize(size);
                }
                buffer->mapData(colors.data(), size);
            }
        }
    }
}

void ColorPicker::render(InstanceRenderer& renderer, [[maybe_unused]] Scene &scene, Context &ctx, const Assets &assets, [[maybe_unused]] const Camera &camera, UniformSetter &setter) {
    if (data.empty()) {
        return;
    }

    framebuffer.bind();

    ctx.enable(Capabilities::DepthTest);
    ctx.setDepthFunc(DepthFunc::Less);
    ctx.setDepthMask(DepthMask::True);

    ctx.clearColor(glm::vec4{0.0f, 0.0f, 0.f, 1.f});

    framebuffer.clear();

    auto isetter = [&](ShaderProgram& shader, const Instance& instance) {
        if (instance.getInstanceType() == InstanceType::Instanced) {
            instanced_buffers[instance.getId()]->bindBase(ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, "color_buffer"));
            return;
        }

        shader.setUniform("color_id", convert(instance.getId()));
    };

    renderer.renderScene({ctx, assets, ShaderType::ColorPicker, ms::Blending::Opaque, setter, {isetter}});

    for (auto& pick: data) {
        if (!pick.sync.isAlreadyPlaced()) {
            pick.sync.place();
        }
    }

    process(ctx);

    framebuffer.unbind();
}

void ColorPicker::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);

    framebuffer << TextureAttachment {FramebufferAttachment::Depth, renderer.getPass<DeferredFramebufferPass>().getDepth()};
}
