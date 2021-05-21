#include <limitless/pipeline/shadow_pass.hpp>

#include <limitless/scene.hpp>
#include <limitless/core/uniform_setter.hpp>

using namespace Limitless;

DirectionalShadowPass::DirectionalShadowPass(RenderPass* prev, Context& ctx, const RenderSettings& settings)
    : RenderPass(prev)
    , shadows {ctx, settings} {
}

DirectionalShadowPass::DirectionalShadowPass(RenderPass *prev, Context& ctx, const RenderSettings& settings, fx::EffectRenderer& renderer)
    : RenderPass(prev)
    , shadows {ctx, settings}
    , effect_renderer {&renderer} {
}

void DirectionalShadowPass::draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, [[maybe_unused]] const UniformSetter& setter) {
    if (light) {
        shadows.draw(instances, *light, ctx, assets, camera, effect_renderer);
        shadows.mapData();
    }
}

void DirectionalShadowPass::addSetter(UniformSetter& setter) {
    setter.add([&] (ShaderProgram& shader) {
        shadows.setUniform(shader);
    });
}

void DirectionalShadowPass::update(Scene& scene, [[maybe_unused]] Instances& instances, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) {
    light = &scene.lighting.directional_light;
}
