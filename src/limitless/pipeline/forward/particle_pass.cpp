#include <limitless/pipeline/forward/particle_pass.hpp>

#include <limitless/fx/effect_renderer.hpp>

using namespace Limitless;

ParticlePass::ParticlePass(Pipeline& pipeline, fx::EffectRenderer& _renderer, ms::Blending _blending)
    : PipelinePass(pipeline)
    , renderer {_renderer}
    , blending {_blending} {
}

void
ParticlePass::draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera,
                   UniformSetter &setter) {
//    renderer.get().draw(ctx, ShaderType::Forward, blending, setter, <#initializer#>);
}