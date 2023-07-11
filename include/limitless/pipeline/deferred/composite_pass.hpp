#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    /**
     * CompositePass combines results from previous passes together
     *
     * result from transparent pass (lighting image + transparent objects) combined with bloom pass result
     */
    class CompositePass final : public PipelinePass {
    private:
        /**
         * Result framebuffer
         */
        Framebuffer framebuffer;
    public:
        explicit CompositePass(Pipeline& pipeline, glm::uvec2 size);

        std::shared_ptr<Texture> getResult() override;

        /**
         *
         */
        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;

        /**
         * Updates framebuffer size
         */
        void onFramebufferChange(glm::uvec2 size) override;
    };
}