#include <limitless/pipeline/effectupdate_pass.hpp>

using namespace Limitless;

EffectUpdatePass::EffectUpdatePass(RenderPass* prev, Context& ctx)
    : RenderPass(prev)
    , renderer {ctx} {
}

void EffectUpdatePass::update([[maybe_unused]] Scene& scene, Instances& instances, [[maybe_unused]] Context& ctx, [[maybe_unused]] const Camera& camera) {
    renderer.update(instances);
}