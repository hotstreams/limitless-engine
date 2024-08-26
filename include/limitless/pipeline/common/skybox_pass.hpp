#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <limitless/renderer/instance_renderer.hpp>

namespace Limitless {
    class Skybox;

    /**
     * SkyboxPass renders skybox from scene
     *
     * note: uses current framebuffer
     */
    class SkyboxPass final : public PipelinePass {
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
        void draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;
    };
}