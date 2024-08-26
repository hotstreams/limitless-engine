#include <limitless/pipeline/common/bloom_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred/deferred_framebuffer_pass.hpp>
#include <iostream>

using namespace Limitless;

BloomPass::BloomPass(Pipeline& pipeline, glm::uvec2 frame_size)
    : PipelinePass {pipeline}
    , bloom {frame_size} {
}

void BloomPass::draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera,
                     UniformSetter &setter) {
    bloom.process(ctx, assets, getPreviousResult());
}

void BloomPass::onFramebufferChange(glm::uvec2 size) {
    bloom.onFramebufferChange(size);
}

std::shared_ptr<Texture> BloomPass::getResult() {
    return bloom.getResult();
}
