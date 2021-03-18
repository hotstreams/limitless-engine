#include <postprocessing.hpp>

#include <shader_storage.hpp>
#include <assets.hpp>
#include <model.hpp>
#include <core/texture_builder.hpp>

using namespace LimitlessEngine;

void Bloom::extractBrightness(const std::shared_ptr<Texture>& image) {
    auto& brightness_shader = *shader_storage.get("brightness");

    brightness.bind();

    brightness_shader << UniformSampler("image", image);

    brightness_shader.use();

    assets.models["quad"]->getMeshes()[0]->draw();
}

void Bloom::blurImage() {
    auto& blur_shader = *shader_storage.get("blur");

    for (uint8_t i = 0; i < blur_iterations; ++i) {
        auto index = i % 2;
        auto direction = index ? glm::vec2{1.0f, 0.0f} : glm::vec2{0.0f, 1.0f};
        auto image = (i == 0) ? brightness.get(FramebufferAttachment::Color0).texture : blur[!index].get(FramebufferAttachment::Color0).texture;

        blur[index].bind();

        blur_shader << UniformValue<glm::vec2>{"direction", direction}
                    << UniformSampler("image", image);

        blur_shader.use();

        assets.models["quad"]->getMeshes()[0]->draw();
    }
}

Bloom::Bloom(ContextEventObserver& ctx)
    : brightness(ctx), blur{ Framebuffer(ctx), Framebuffer(ctx) } {

    auto param_set = [] (Texture& texture) {
        texture << TexParameter<GLint>{GL_TEXTURE_MAG_FILTER, GL_LINEAR}
                << TexParameter<GLint>{GL_TEXTURE_MIN_FILTER, GL_LINEAR}
                << TexParameter<GLint>{GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE}
                << TexParameter<GLint>{GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE};
    };
    auto texture = TextureBuilder::build(Texture::Type::Tex2D, 1, Texture::InternalFormat::RGB16F, ctx.getSize(), Texture::Format::RGB, Texture::DataType::Float, nullptr, param_set);


    brightness << TextureAttachment{FramebufferAttachment::Color0, texture};
    brightness.drawBuffer(FramebufferAttachment::Color0);
    brightness.checkStatus();

    for (auto& fbo : blur) {
        auto tex = TextureBuilder::build(Texture::Type::Tex2D, 1, Texture::InternalFormat::RGB16F, ctx.getSize(), Texture::Format::RGB, Texture::DataType::Float, nullptr, param_set);

        fbo << TextureAttachment{FramebufferAttachment::Color0, tex};
        fbo.drawBuffer(FramebufferAttachment::Color0);
        fbo.checkStatus();

        fbo.unbind();
    }
}

void Bloom::process(const std::shared_ptr<Texture>& image) {
    extractBrightness(image);
    blurImage();
}

const std::shared_ptr<Texture>& Bloom::getResult() const noexcept {
    return blur[(blur_iterations - 1) % 2].get(FramebufferAttachment::Color0).texture;
}

PostProcessing::PostProcessing(ContextEventObserver& ctx) : bloom_process(ctx) {

}

void PostProcessing::process(Context& context, const Framebuffer& offscreen) {
    auto& postprocess_shader = *shader_storage.get("postprocess");

    context.disable(Capabilities::DepthTest);
    context.setDepthMask(DepthMask::True);
    context.disable(Capabilities::Blending);

    if (bloom) {
        bloom_process.process(offscreen.get(FramebufferAttachment::Color0).texture);
    }

    Framebuffer::bindDefault();
    context.clear(Clear::ColorDepthStencil);

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

    assets.models["quad"]->getMeshes()[0]->draw();
}
