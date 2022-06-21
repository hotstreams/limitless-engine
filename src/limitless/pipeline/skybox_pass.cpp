#include <limitless/pipeline/skybox_pass.hpp>

#include <limitless/skybox/skybox.hpp>
#include <limitless/scene.hpp>
#include <limitless/ms/material.hpp>

using namespace Limitless;

void SkyboxPass::draw([[maybe_unused]] Instances& instances, Context& ctx, const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    if (skybox) {
    	skybox->getMaterial().update();
        skybox->draw(ctx, assets);
    }
}

void SkyboxPass::update(Scene& scene, [[maybe_unused]] Instances& instances, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) {
    skybox = scene.getSkybox().get();
}

SkyboxPass::SkyboxPass(Pipeline& pipeline)
    : RenderPass(pipeline) {
}
