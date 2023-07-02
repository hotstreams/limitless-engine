#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    /**
     * DeferredLightingPass makes lighting calculation using data from GBUFFER and renders result to its own framebuffer
     */
    class DeferredLightingPass final : public RenderPass {
    private:
        /**
         * Lighting result framebuffer
         */
        Framebuffer framebuffer;
    public:
        explicit DeferredLightingPass(Pipeline& pipeline, glm::uvec2 frame_size);

        auto& getFramebuffer() noexcept { return framebuffer; }

        std::shared_ptr<Texture> getResult() override;

        /**
         * Renders lighting to framebuffer
         */
        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;

        /**
         * Updates framebuffer size
         */
        void onFramebufferChange(glm::uvec2 size) override;
    };
}
