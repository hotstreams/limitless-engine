#include <limitless/renderer/skybox_pass.hpp>

#include <limitless/skybox/skybox.hpp>
#include <limitless/scene.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/core/framebuffer.hpp>
#include <limitless/renderer/deferred_framebuffer_pass.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/core/profiler.hpp>

using namespace Limitless;

SkyboxPass::SkyboxPass(Renderer& renderer)
    : RendererPass {renderer} {
}

void SkyboxPass::render([[maybe_unused]] InstanceRenderer &instance_renderer, Scene &scene, Context &ctx, const Assets &assets, [[maybe_unused]] const Camera &camera, [[maybe_unused]] UniformSetter &setter) {
    ProfilerScope profile_scope {"SkyboxPass"};
    auto& gbuffer = renderer.getPass<DeferredFramebufferPass>();

    // Skybox needs only a minimal set of MRTs
    gbuffer.getFramebuffer().drawBuffers({
         FramebufferAttachment::Color0, // albedo
         FramebufferAttachment::Color3  // emissive (if used by skybox)
    });

    if (auto skybox = scene.getSkybox(); skybox) {
        //TODO: move?
        skybox->getMaterial().update();
        skybox->draw(ctx, assets);
    }
}
