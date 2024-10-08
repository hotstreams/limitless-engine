#include <limitless/postprocessing/bloom.hpp>

#include "limitless/core/shader/shader_program.hpp"
#include "limitless/core/uniform/uniform.hpp"
#include <limitless/assets.hpp>

using namespace Limitless;

void Bloom::extractBrightness(const Assets& assets, const std::shared_ptr<Texture>& image) {
    auto& brightness_shader = assets.shaders.get("brightness");

    brightness.clear();

    brightness_shader .setUniform("image", image)
                      .setUniform("threshold", threshold);
    brightness_shader.use();

    assets.meshes.at("quad")->draw();
}

Bloom::Bloom([[maybe_unused]] const RendererSettings& settings, glm::uvec2 resolution)
    : blur {resolution}
    , brightness {Framebuffer::asRGB16FNearestClampToEdge(resolution)} {
}

void Bloom::process(Context& ctx, const Assets& assets, const std::shared_ptr<Texture>& image) {
    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::StencilTest);
    ctx.disable(Capabilities::Blending);

    extractBrightness(assets, image);
    blur.process(ctx, assets, brightness.get(FramebufferAttachment::Color0).texture);
}

const std::shared_ptr<Texture>& Bloom::getResult() const noexcept {
    return blur.getResult();
}

void Bloom::onFramebufferChange(glm::uvec2 frame_size) {
    brightness.onFramebufferChange(frame_size);
    blur.onFramebufferChange(frame_size);
}
