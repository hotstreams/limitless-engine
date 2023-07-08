#include <limitless/pipeline/deferred/effectupdate_pass.hpp>

using namespace Limitless;

EffectUpdatePass::EffectUpdatePass(Pipeline& pipeline, Context& ctx)
    : PipelinePass(pipeline)
    , renderer {ctx} {
}

void EffectUpdatePass::update([[maybe_unused]] Scene& scene, Instances& instances, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) {
    renderer.update(instances);
}