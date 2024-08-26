#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <limitless/postprocessing/bloom.hpp>

namespace Limitless {
    /**
     * BloomPass implements Bloom for an image
     *
     * It extracts color values within some defined threshold and then applies Down-Up-sampling blur
     *
     * note: as an input image uses previous pass result
     */
    class BloomPass final : public PipelinePass {
    private:
        /**
         * Bloom implementation
         */
        Bloom bloom;
    public:
        BloomPass(Pipeline& pipeline, glm::uvec2 frame_size);

        std::shared_ptr<Texture> getResult() override;
        auto& getBloom() noexcept { return bloom; }

        /**
         * Makes image bloom-ish
         */
        void
        draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera,
             UniformSetter &setter) override;

        /**
         * Update framebuffer size
         */
        void onFramebufferChange(glm::uvec2 size) override;
    };
}