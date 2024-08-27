#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/lighting/cascade_shadows.hpp>
#include <limitless/renderer/instance_renderer.hpp>

namespace Limitless {
    /**
     * DirectionalShadowPass renders all objects and effects that casts shadow in shadow map
     */
    class DirectionalShadowPass final : public RendererPass {
    private:
        /**
         * CSM implementation
         */
        CascadeShadows shadows;
    public:
        explicit DirectionalShadowPass(Renderer& renderer);

        /**
         * Adds shadow-specific uniforms to setter
         *
         * TODO: can be removed if implement this values in uniform buffer
         */
        void addUniformSetter(UniformSetter& setter) override;

        /**
         * Draws shadow
         */
        void render(InstanceRenderer &renderer, Scene& scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;
    };
}