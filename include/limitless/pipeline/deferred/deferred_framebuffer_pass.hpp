#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
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
        auto& getInfo() noexcept { return framebuffer.get(FramebufferAttachment::Color4).texture; }
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