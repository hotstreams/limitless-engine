#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/pipeline/common/scene_data.hpp>

namespace Limitless {
    /**
     * Updates global info about the whole scene and context
     */
    class SceneUpdatePass final : public RenderPass {
    private:
        SceneDataStorage scene_data;
    public:
        SceneUpdatePass(Pipeline& pipeline, Context& ctx);

        /**
         * Updates scene, maps scene info buffer to GPU, sets up instances
         */
        void update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera) override;

        /**
         * Updates scene size
         *
         * used for postprocess effects mostly
         */
        void onFramebufferChange(glm::uvec2 size) override;
    };
}