#include <limitless/pipeline/postprocessing.hpp>

#include <limitless/models/model.hpp>
#include <limitless/core/texture_builder.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/core/uniform.hpp>

#include <limitless/models/abstract_mesh.hpp>

using namespace Limitless;

Blur::Blur(ContextEventObserver& ctx)
    : blur { Framebuffer::asRGB16FLinearClampToEdge(ctx), Framebuffer::asRGB16FLinearClampToEdge(ctx) } {
}

Blur::Blur(glm::uvec2 frame_size) {
    blur[0] = Framebuffer::asRGB16FLinearClampToEdge(frame_size);
    blur[1] = Framebuffer::asRGB16FLinearClampToEdge(frame_size);
}

void Blur::onResize(glm::uvec2 frame_size) {
    blur[0].onFramebufferChange(frame_size);
    blur[1].onFramebufferChange(frame_size);
}

void Blur::process(const Assets& assets, const std::shared_ptr<Texture>& t) {
    auto& blur_shader = assets.shaders.get("blur");

    for (uint8_t i = 0; i < 8; ++i) {
        auto index = i % 2;
        auto direction = index ? glm::vec2{1.0f, 0.0f} : glm::vec2{0.0f, 1.0f};
        auto image = (i == 0) ? t : blur[!index].get(FramebufferAttachment::Color0).texture;

        blur[index].bind();

        blur_shader << UniformValue<glm::vec2>{"direction", direction}
                    << UniformSampler("image", image);

        blur_shader.use();

        assets.meshes.at("quad")->draw();
    }
}

const std::shared_ptr<Texture>& Blur::getResult() const noexcept {
    return blur[(8 - 1) % 2].get(FramebufferAttachment::Color0).texture;
}

void Bloom::extractBrightness(const Assets& assets, const std::shared_ptr<Texture>& image) {
    auto& brightness_shader = assets.shaders.get("brightness");

    brightness.bind();

    brightness_shader << UniformSampler("image", image);

    brightness_shader.use();

    assets.meshes.at("quad")->draw();
}

Bloom::Bloom(ContextEventObserver& ctx)
    : brightness {Framebuffer::asRGB16FLinearClampToEdge(ctx)}
    , blur {ctx} {
}

Bloom::Bloom(glm::uvec2 frame_size)
    : brightness {Framebuffer::asRGB16FLinearClampToEdge(frame_size)}
    , blur {frame_size} {
}

void Bloom::process(const Assets& assets, const std::shared_ptr<Texture>& image) {
    extractBrightness(assets, image);
    blur.process(assets, brightness.get(FramebufferAttachment::Color0).texture);
}

const std::shared_ptr<Texture>& Bloom::getResult() const noexcept {
    return blur.getResult();
}

void Bloom::onResize(glm::uvec2 frame_size) {
    brightness.onFramebufferChange(frame_size);
    blur.onResize(frame_size);
}

PostProcessing::PostProcessing(ContextEventObserver& ctx, RenderTarget& _target)
    : target {_target}
    , bloom_process(ctx) {
}

PostProcessing::PostProcessing(glm::uvec2 frame_size, RenderTarget& _target)
    : target {_target}
    , bloom_process {frame_size} {
}

void PostProcessing::process(Context& ctx, const Assets& assets, const Framebuffer& offscreen) {
    auto& postprocess_shader = assets.shaders.get("postprocess");

    ctx.disable(Capabilities::DepthTest);
    ctx.setDepthMask(DepthMask::True);
    ctx.disable(Capabilities::Blending);

    if (bloom) {
        bloom_process.process(assets, offscreen.get(FramebufferAttachment::Color0).texture);
    }

    target.clear();

    postprocess_shader << UniformValue("bloom", static_cast<int>(bloom))
                       << UniformSampler("bloom_image", bloom_process.getResult())
                       << UniformValue("tone_mapping", static_cast<int>(tone_mapping))
                       << UniformValue("exposure", exposure)
                       << UniformValue("gamma_correction", static_cast<int>(gamma_correction))
                       << UniformValue("gamma", gamma)
                       << UniformValue("vignette", static_cast<int>(vignette))
                       << UniformValue("vignette_radius", vignette_radius)
                       << UniformValue("vignette_softness", vignette_softness)
                       << UniformValue("tone_shading", static_cast<int>(tone_shading))
                       << UniformValue("number_of_colors", number_of_colors)
                       << UniformValue("line_texel_offset", line_texel_offset)
                       << UniformValue("line_multiplier", line_multiplier)
                       << UniformValue("line_bias", line_bias)
                       << UniformValue("fxaa", static_cast<int>(fxaa))
                       << UniformSampler("image", offscreen.get(FramebufferAttachment::Color0).texture)
                       << UniformSampler("image_depth", offscreen.get(FramebufferAttachment::Depth).texture);

    postprocess_shader.use();

    assets.meshes.at("quad")->draw();

    target.unbind();
}

void PostProcessing::onResize(glm::uvec2 frame_size) {
    bloom_process.onResize(frame_size);
}
