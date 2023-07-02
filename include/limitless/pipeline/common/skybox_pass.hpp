#pragma once

#include <limitless/pipeline/render_pass.hpp>

namespace Limitless {
    class Skybox;

    /**
     * SkyboxPass renders skybox from scene
     *
     * note: uses current framebuffer
     */
    class SkyboxPass final : public RenderPass {
    private:
        /**
         * Current skybox
         */
        Skybox* skybox {};
    public:
        explicit SkyboxPass(Pipeline& pipeline);

        /**
         * Updates current skybox
         */
        void update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera) override;

        /**
         * Renders skybox to current bound framebuffer
         */
        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}