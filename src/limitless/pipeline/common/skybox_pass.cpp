#include <limitless/pipeline/common/skybox_pass.hpp>

#include <limitless/skybox/skybox.hpp>
#include <limitless/scene.hpp>
#include <limitless/ms/material.hpp>

using namespace Limitless;

SkyboxPass::SkyboxPass(Pipeline& pipeline)
    : PipelinePass(pipeline) {
}

void SkyboxPass::draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) {
    if (skybox) {
    	skybox->getMaterial().update();
        skybox->draw(ctx, assets);
    }
}

void SkyboxPass::update(Scene& scene, [[maybe_unused]] Instances& instances, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) {
    skybox = scene.getSkybox().get();
}
