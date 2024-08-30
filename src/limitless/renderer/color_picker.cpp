#include <limitless/renderer/color_picker.hpp>

#include <limitless/renderer/shader_type.hpp>
#include <utility>
#include <limitless/core/uniform/uniform_sampler.hpp>
#include <limitless/renderer/instance_renderer.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/renderer/depth_pass.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>

using namespace Limitless;

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

        uint8_t color[3];
        glReadPixels(info.coords.x, info.coords.y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, color);

        framebuffer.unbind();

        info.callback(convert({color[0], color[1], color[2]}));

        data.pop_front();
    }
}

void ColorPicker::render(InstanceRenderer& renderer, [[maybe_unused]] Scene &scene, Context &ctx, const Assets &assets, [[maybe_unused]] const Camera &camera, UniformSetter &setter) {
    if (data.empty()) {
        return;
    }

    framebuffer.bind();

    ctx.enable(Capabilities::DepthTest);
    ctx.setDepthFunc(DepthFunc::Equal);
    ctx.setDepthMask(DepthMask::False);

    ctx.clearColor(glm::vec4{0.0f, 0.0f, 0.f, 1.f});

    framebuffer.clear();

    renderer.renderScene({ctx, assets, ShaderType::ColorPicker, ms::Blending::Opaque, setter});

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
