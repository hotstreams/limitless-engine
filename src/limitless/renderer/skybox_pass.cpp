#include <limitless/renderer/skybox_pass.hpp>

#include <limitless/skybox/skybox.hpp>
#include <limitless/scene.hpp>
#include <limitless/ms/material.hpp>

using namespace Limitless;

SkyboxPass::SkyboxPass(Renderer& renderer)
    : RendererPass {renderer} {
}

void SkyboxPass::render([[maybe_unused]] InstanceRenderer &instance_renderer, Scene &scene, Context &ctx, const Assets &assets, [[maybe_unused]] const Camera &camera, [[maybe_unused]] UniformSetter &setter) {
    if (auto skybox = scene.getSkybox(); skybox) {
        //TODO: move?
        skybox->getMaterial().update();
        skybox->draw(ctx, assets);
    }
}
