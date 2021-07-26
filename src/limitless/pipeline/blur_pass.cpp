#include <limitless/pipeline/blur_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred_framebuffer_pass.hpp>

using namespace Limitless;

BlurPass::BlurPass(Pipeline& pipeline, ContextEventObserver& ctx)
    : RenderPass(pipeline)
    , blur {ctx} {
}

void BlurPass::draw([[maybe_unused]] Instances& instances, [[maybe_unused]] Context& ctx, const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    blur.process(assets, pipeline.get<DeferredFramebufferPass>().getEmissive());
}
