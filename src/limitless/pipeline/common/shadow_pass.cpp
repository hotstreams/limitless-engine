#include <limitless/pipeline/common/shadow_pass.hpp>

#include <limitless/scene.hpp>
#include <limitless/core/uniform/uniform_setter.hpp>
#include <limitless/renderer/instance_renderer.hpp>

using namespace Limitless;

DirectionalShadowPass::DirectionalShadowPass(Pipeline& pipeline, Context& ctx, const RendererSettings& settings)
    : PipelinePass(pipeline)
    , shadows {ctx, settings} {
}

void DirectionalShadowPass::draw(InstanceRenderer &renderer, Scene& scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) {
    if (light) {
        shadows.draw(renderer, scene, *light, ctx, assets, camera);
        shadows.mapData();
    }
}

void DirectionalShadowPass::addSetter(UniformSetter& setter) {
    setter.add([&] (ShaderProgram& shader) {
        shadows.setUniform(shader);
    });
}

void DirectionalShadowPass::update(Scene& scene, [[maybe_unused]] Instances& instances, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) {
    light = &scene.getLighting().getDirectionalLight();
}
