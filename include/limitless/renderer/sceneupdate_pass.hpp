#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/renderer/scene_data.hpp>

namespace Limitless {
    /**
     * Updates global info about the whole scene and context
     */
    class SceneUpdatePass final : public RendererPass {
    private:
        SceneDataStorage scene_data;
    public:
        explicit SceneUpdatePass(Renderer& renderer);

        /**
         * Updates scene, maps scene info buffer to GPU, sets up instances
         */
        void update(Scene &scene, const Camera &camera) override;

        /**
         * Updates scene size
         *
         * used for postprocess effects mostly
         */
        void onFramebufferChange(glm::uvec2 size) override;
    };
}