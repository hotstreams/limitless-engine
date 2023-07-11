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
    private:
        /**
         * EffectRenderer to render opaque effects
         */
        fx::EffectRenderer& renderer;
    public:
        GBufferPass(Pipeline& pipeline, fx::EffectRenderer& renderer);
        ~GBufferPass() override = default;

        /**
         * Fills GBUFFER with opaque objects and effects data
         */
        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}