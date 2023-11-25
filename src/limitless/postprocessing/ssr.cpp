#include <limitless/postprocessing/ssr.hpp>

#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/core/uniform/uniform_setter.hpp>
#include <limitless/assets.hpp>
#include <limitless/camera.hpp>

using namespace Limitless;

SSR::SSR(ContextEventObserver& ctx)
    : blur {ctx.getSize()} {
    auto ssr = Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::RGB16F)
            .data_type(Texture::DataType::Float)
            .size(ctx.getSize())
            .min_filter(Texture::Filter::Nearest)
            .mag_filter(Texture::Filter::Nearest)
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .build();

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, ssr};
    framebuffer.checkStatus();
    framebuffer.unbind();
}

SSR::SSR(ContextEventObserver &ctx, glm::uvec2 frame_size)
    : blur {frame_size} {
    auto ssr = Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::RGB16F)
            .data_type(Texture::DataType::Float)
            .size(ctx.getSize())
            .min_filter(Texture::Filter::Nearest)
            .mag_filter(Texture::Filter::Nearest)
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .build();

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, ssr};
    framebuffer.checkStatus();
    framebuffer.unbind();
}

void SSR::draw(Context& ctx, const Assets& assets, const Camera& camera,
               const std::shared_ptr<Texture>& depth,
               const std::shared_ptr<Texture>& normal,
               const std::shared_ptr<Texture>& props,
               const std::shared_ptr<Texture>& image) {
    {
        ctx.disable(Capabilities::DepthTest);
        ctx.disable(Capabilities::Blending);

        framebuffer.drawBuffers({FramebufferAttachment::Color0});
        framebuffer.clear();
    }

    {
        framebuffer.drawBuffer(FramebufferAttachment::Color0);

        auto& shader = assets.shaders.get("ssr");

        shader.setUniform("depth_texture", depth)
              .setUniform("normal_texture", normal)
              .setUniform("props_texture", props)
              .setUniform("base_color_texture", image)

              .setUniform("vs_thickness", settings.vs_thickness)
              .setUniform("stride", settings.stride)
              .setUniform("vs_max_distance", settings.vs_max_distance)
              .setUniform("bias", settings.bias)
              .setUniform("max_steps", settings.max_steps)
              .setUniform("reflection_threshold", settings.reflection_threshold)
              .setUniform("roughness_factor", settings.roughness_factor)
              .setUniform("reflection_strength", settings.reflection_strength)
              .setUniform("reflection_falloff_exp", settings.reflection_falloff_exp)
              .setUniform("camera_attenuation_lower_edge", settings.camera_attenuation_lower_edge)
              .setUniform("camera_attenuation_upper_edge", settings.camera_attenuation_upper_edge);

        shader.use();

        assets.meshes.at("quad")->draw();
    }

    if (settings.reflection_blur) {
        blur.process(ctx, assets, framebuffer.get(FramebufferAttachment::Color0).texture);
    }

    update();
}

void SSR::onFramebufferChange(glm::uvec2 frame_size) {
    framebuffer.onFramebufferChange(frame_size);
    blur.onFramebufferChange(frame_size);
}

std::shared_ptr<Texture> SSR::getResult() {
    return settings.reflection_blur ? blur.getResult() : framebuffer.get(FramebufferAttachment::Color0).texture;
}

void SSR::update() {
    settings.reflection_strength = 1.0 / blur.getIterationCount();
}

void SSR::addSetter(UniformSetter& setter) {
    setter.add([&](ShaderProgram& shader) {
        shader.setUniform("_ssr_strength", settings.reflection_strength);
    });
}