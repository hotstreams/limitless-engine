#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    /**
     * DeferredFramebufferPass sets up GBUFFER for Deferred pipeline to render object to
     *
     * Draw buffer 0:
     *      UNSIGNED NORMALIZED [0; 1]
     *      RGBA16 - RGB - base color; A - ao
     *
     * Draw buffer 1:
     *      SIGNED NORMALIZED [-1; 1]
     *      RGB16 - normal
     * TODO: RGB24?
     * Draw buffer 2:
     *      UNSIGNED NORMALIZED [0; 1]
     *      RGBA16 - R - roughness, G - metallic, B - shading model (uint)
     *
     * Draw buffer 3:
     *      FLOATING POINT
     *      RGB16F - emissive
     *
     * Depth buffer:
     *      DEPTH32F
     */
    class DeferredFramebufferPass final : public PipelinePass {
    private:
        Framebuffer framebuffer;
    public:
        DeferredFramebufferPass(Pipeline& pipeline, glm::uvec2 frame_size);

        /**
         * Gets GBUFFER textures
         */
        auto& getFramebuffer() noexcept { return framebuffer; }
        auto& getAlbedo() noexcept { return framebuffer.get(FramebufferAttachment::Color0).texture; }
        auto& getNormal() noexcept { return framebuffer.get(FramebufferAttachment::Color1).texture; }
        auto& getProperties() noexcept { return framebuffer.get(FramebufferAttachment::Color2).texture; }
        auto& getEmissive() noexcept { return framebuffer.get(FramebufferAttachment::Color3).texture; }
        auto& getDepth() noexcept { return framebuffer.get(FramebufferAttachment::Depth).texture; }

        /**
         * Sets up context state, binds framebuffer
         */
        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;

        /**
         * Updates framebuffer size
         */
        void onFramebufferChange(glm::uvec2 size) override;
    };
}