#include <limitless/renderer/fxaa_pass.hpp>

#include <limitless/core/context.hpp>
#include "limitless/core/uniform/uniform.hpp"
#include <limitless/assets.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include <limitless/renderer/composite_pass.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>

using namespace Limitless;

FXAAPass::FXAAPass(Renderer& renderer)
    : RendererPass(renderer)
    , framebuffer {Framebuffer::asRGB8LinearClampToEdge(renderer.getResolution())} {
}

std::shared_ptr<Texture> FXAAPass::getResult() {
    return framebuffer.get(FramebufferAttachment::Color0).texture;
}

void FXAAPass::render(InstanceRenderer &instance_renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) {
    ctx.disable(Capabilities::DepthTest);
    ctx.disable(Capabilities::Blending);

    {
        framebuffer.clear();
        auto& shader = assets.shaders.get("fxaa");

        shader.setUniform("scene", renderer.getPass<CompositePass>().getResult());

        shader.use();

        assets.meshes.at("quad")->draw();
    }
}

void FXAAPass::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);
}
