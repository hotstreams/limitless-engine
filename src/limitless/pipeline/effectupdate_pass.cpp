#include <limitless/pipeline/effectupdate_pass.hpp>

using namespace Limitless;

EffectUpdatePass::EffectUpdatePass(Pipeline& pipeline, Context& ctx)
    : RenderPass(pipeline)
    , renderer {ctx} {
}

void EffectUpdatePass::update([[maybe_unused]] Scene& scene,
                              Instances& instances,
                              [[maybe_unused]] Context& ctx,
                              [[maybe_unused]] glm::uvec2 frame_size,
                              [[maybe_unused]] const Camera& camera) {
    renderer.update(instances);
}