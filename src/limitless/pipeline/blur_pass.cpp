#include <limitless/pipeline/blur_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred_framebuffer_pass.hpp>

using namespace Limitless;

BlurPass::BlurPass(Pipeline& pipeline, ContextEventObserver& ctx)
    : RenderPass(pipeline)
    , blur {ctx} {
}

BlurPass::BlurPass(Pipeline& pipeline, ContextEventObserver& ctx, glm::uvec2 frame_size)
    : RenderPass {pipeline}
    , blur {frame_size} {
}

void BlurPass::draw([[maybe_unused]] Instances& instances, [[maybe_unused]] Context& ctx, const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    ctx.disable(Capabilities::Blending);
    blur.process(assets, pipeline.get<DeferredFramebufferPass>().getEmissive());
}

void BlurPass::update(Scene& scene, Instances& instances, Context& ctx, glm::uvec2 frame_size, const Camera& camera) {
    blur.onResize(frame_size);
}
