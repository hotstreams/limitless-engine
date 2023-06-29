#include <limitless/pipeline/bloom.hpp>

#include <limitless/core/shader_program.hpp>
#include <limitless/core/uniform/uniform.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/uniform/uniform_sampler.hpp>
using namespace Limitless;

void Bloom::extractBrightness(const Assets& assets, const std::shared_ptr<Texture>& image) {
    auto& brightness_shader = assets.shaders.get("brightness");

    brightness.clear();

    brightness_shader << UniformSampler("image", image)
                      << UniformValue("threshold", threshold);
    brightness_shader.use();

    assets.meshes.at("quad")->draw();
}

Bloom::Bloom(glm::uvec2 frame_size)
    : blur {frame_size}
    , brightness {Framebuffer::asRGB16FNearestClampToEdge(frame_size)} {
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
