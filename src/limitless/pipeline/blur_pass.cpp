#include <limitless/pipeline/blur_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred_framebuffer_pass.hpp>
#include <iostream>

using namespace Limitless;

BlurPass::BlurPass(Pipeline& pipeline, glm::uvec2 frame_size)
    : RenderPass {pipeline}
    , blur {frame_size} {
}

void BlurPass::draw([[maybe_unused]] Instances& instances, [[maybe_unused]] Context& ctx, const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    ctx.disable(Capabilities::Blending);
    blur.process(assets, pipeline.get<DeferredFramebufferPass>().getEmissive());
}

void BlurPass::update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera) {
}

void BlurPass::onFramebufferChange(glm::uvec2 size) {
    blur.onResize(size);
}
