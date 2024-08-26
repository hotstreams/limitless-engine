#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless::fx {
    class EffectRenderer;
}

namespace Limitless {
    /**
     * GBufferPass renders opaque objects to DeferredFramebufferPass's framebuffer to populate material data
     * used for later light calculations
     */
    class GBufferPass final : public PipelinePass {
    public:
        GBufferPass(Pipeline& pipeline);
        ~GBufferPass() override = default;

        /**
         * Fills GBUFFER with opaque objects and effects data
         */
        void draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;
    };
}