#include <limitless/renderer/outline_pass.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/instances/model_instance.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>
#include <limitless/core/cpu_profiler.hpp>

#include "limitless/core/profiler.hpp"

using namespace Limitless;

OutlinePass::OutlinePass(Renderer& renderer)
	: RendererPass(renderer) {
    auto albedo = Texture::builder()
        .target(Texture::Type::Tex2D)
        .internal_format(Texture::InternalFormat::RGB8)
        .format(Texture::Format::RGB)
        .data_type(Texture::DataType::UnsignedByte)
        .size(renderer.getResolution())
        .min_filter(Texture::Filter::Nearest)
        .mag_filter(Texture::Filter::Nearest)
        .wrap_s(Texture::Wrap::ClampToEdge)
        .wrap_t(Texture::Wrap::ClampToEdge)
        .build();

    framebuffer.bind();
    framebuffer << TextureAttachment{FramebufferAttachment::Color0, albedo};
    framebuffer.checkStatus();
    framebuffer.unbind();
    std::srand(std::time(nullptr)); // use current time as seed for random generator

}

void OutlinePass::render(
        [[maybe_unused]] InstanceRenderer &instance_renderer,
        [[maybe_unused]] Scene &scene,
        Context &ctx,
        const Assets &assets,
        [[maybe_unused]] const Camera &camera,
        [[maybe_unused]] UniformSetter &setter) {
    ProfilerScope profile_scope {"OutlinePass"};

    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    framebuffer.clear();

    auto& shader = assets.shaders.get("outline");

    shader
        .setUniform("outline_texture", renderer.getPass<DeferredFramebufferPass>().getOutline())
        .setUniform("width", width)
        .use();

    assets.meshes.at("quad")->draw();
}

std::shared_ptr<Texture> OutlinePass::getResult() {
    return framebuffer.get(FramebufferAttachment::Color0).texture;
}

void OutlinePass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);
}
