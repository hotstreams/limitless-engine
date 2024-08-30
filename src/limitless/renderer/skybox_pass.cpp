#include <limitless/renderer/skybox_pass.hpp>

#include <limitless/skybox/skybox.hpp>
#include <limitless/scene.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/core/framebuffer.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>
#include <limitless/renderer/renderer.hpp>

using namespace Limitless;

SkyboxPass::SkyboxPass(Renderer& renderer)
    : RendererPass {renderer} {
}

void SkyboxPass::render([[maybe_unused]] InstanceRenderer &instance_renderer, Scene &scene, Context &ctx, const Assets &assets, [[maybe_unused]] const Camera &camera, [[maybe_unused]] UniformSetter &setter) {
    auto& gbuffer = renderer.getPass<DeferredFramebufferPass>();

    gbuffer.getFramebuffer().drawBuffers({
         FramebufferAttachment::Color0,
         FramebufferAttachment::Color1,
         FramebufferAttachment::Color2,
         FramebufferAttachment::Color3,
         FramebufferAttachment::Color4
    });

    if (auto skybox = scene.getSkybox(); skybox) {
        //TODO: move?
        skybox->getMaterial().update();
        skybox->draw(ctx, assets);
    }
}
