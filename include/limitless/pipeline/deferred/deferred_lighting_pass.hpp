#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <limitless/core/framebuffer.hpp>
#include <limitless/renderer/instance_renderer.hpp>

namespace Limitless {
    /**
     * DeferredLightingPass makes lighting calculation using data from GBUFFER and renders result to its own framebuffer
     */
    class DeferredLightingPass final : public PipelinePass {
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
        void
        draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera,
             UniformSetter &setter) override;

        /**
         * Updates framebuffer size
         */
        void onFramebufferChange(glm::uvec2 size) override;
    };
}
