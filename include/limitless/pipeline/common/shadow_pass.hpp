#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <limitless/lighting/cascade_shadows.hpp>
#include <limitless/renderer/instance_renderer.hpp>

namespace Limitless::fx {
    class EffectRenderer;
}

namespace Limitless {
    /**
     * DirectionalShadowPass renders all objects and effects that casts shadow in shadow map
     */
    class DirectionalShadowPass final : public PipelinePass {
    private:
        /**
         * CSM implementation
         */
        CascadeShadows shadows;

        /**
         * Current direction light used to shadow cast
         */
        Light* light {};
    public:
        DirectionalShadowPass(Pipeline& pipeline, Context& ctx, const RendererSettings& settings);

        /**
         * Adds shadow-specific uniforms to setter
         *
         * TODO: can be removed if implement this values in uniform buffer
         */
        void addSetter(UniformSetter& setter) override;

        /**
         * Sets current directional light
         */
        void update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera) override;

        /**
         * Draws shadow
         */
        void draw(InstanceRenderer &renderer, Scene& scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;
    };
}