#include <limitless/renderer/outline_pass.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/instances/model_instance.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>

using namespace Limitless;

OutlinePass::OutlinePass(Renderer& renderer)
	: RendererPass(renderer) {
    auto albedo = Texture::Builder::asRGB16NearestClampToEdge(renderer.getResolution());

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
    GPUProfileScope profile_scope {global_gpu_profiler, "OutlinePass"};

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
