#include <limitless/pipeline/postprocessing.hpp>

#include <limitless/models/model.hpp>
#include <limitless/core/texture_builder.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/core/uniform.hpp>

#include <limitless/models/abstract_mesh.hpp>

using namespace Limitless;

void Bloom::extractBrightness(const Assets& assets, const std::shared_ptr<Texture>& image) {
    auto& brightness_shader = assets.shaders.get("brightness");

    brightness.bind();

    brightness_shader << UniformSampler("image", image);

    brightness_shader.use();

    assets.meshes.at("quad")->draw();
}

void Bloom::blurImage(const Assets& assets) {
    auto& blur_shader = assets.shaders.get("blur");

    for (uint8_t i = 0; i < blur_iterations; ++i) {
        auto index = i % 2;
        auto direction = index ? glm::vec2{1.0f, 0.0f} : glm::vec2{0.0f, 1.0f};
        auto image = (i == 0) ? brightness.get(FramebufferAttachment::Color0).texture : blur[!index].get(FramebufferAttachment::Color0).texture;

        blur[index].bind();

        blur_shader << UniformValue<glm::vec2>{"direction", direction}
                    << UniformSampler("image", image);

        blur_shader.use();

        assets.meshes.at("quad")->draw();
    }
}

Bloom::Bloom(ContextEventObserver& ctx)
    : brightness {ctx}
    , blur { Framebuffer(ctx), Framebuffer(ctx) } {

    {
        TextureBuilder builder;
        auto texture = builder.setTarget(Texture::Type::Tex2D)
                              .setInternalFormat(Texture::InternalFormat::RGB16F)
                              .setSize(ctx.getSize())
                              .setFormat(Texture::Format::RGB)
                              .setDataType(Texture::DataType::Float)
                              .setMinFilter(Texture::Filter::Linear)
                              .setMagFilter(Texture::Filter::Linear)
                              .setWrapS(Texture::Wrap::ClampToEdge)
                              .setWrapT(Texture::Wrap::ClampToEdge)
                              .build();

        brightness << TextureAttachment{FramebufferAttachment::Color0, texture};
        brightness.drawBuffer(FramebufferAttachment::Color0);
        brightness.checkStatus();
    }

    for (auto& fbo : blur) {
        TextureBuilder builder;
        auto texture = builder.setTarget(Texture::Type::Tex2D)
                              .setInternalFormat(Texture::InternalFormat::RGB16F)
                              .setSize(ctx.getSize())
                              .setFormat(Texture::Format::RGB)
                              .setDataType(Texture::DataType::Float)
                              .setMinFilter(Texture::Filter::Linear)
                              .setMagFilter(Texture::Filter::Linear)
                              .setWrapS(Texture::Wrap::ClampToEdge)
                              .setWrapT(Texture::Wrap::ClampToEdge)
                              .build();
        fbo << TextureAttachment{FramebufferAttachment::Color0, texture};
        fbo.drawBuffer(FramebufferAttachment::Color0);
        fbo.checkStatus();

        fbo.unbind();
    }
}

void Bloom::process(const Assets& assets, const std::shared_ptr<Texture>& image) {
    extractBrightness(assets, image);
    blurImage(assets);
}

const std::shared_ptr<Texture>& Bloom::getResult() const noexcept {
    return blur[(blur_iterations - 1) % 2].get(FramebufferAttachment::Color0).texture;
}

PostProcessing::PostProcessing(ContextEventObserver& ctx, RenderTarget& _target)
    : target {_target}
    , bloom_process(ctx) {
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
