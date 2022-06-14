#include <limitless/pipeline/effectupdate_pass.hpp>

using namespace Limitless;

EffectUpdatePass::EffectUpdatePass(Pipeline& pipeline, Context& ctx)
    : RenderPass(pipeline)
    , renderer {ctx} {
}

void EffectUpdatePass::update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera) {
    renderer.update(instances);
}