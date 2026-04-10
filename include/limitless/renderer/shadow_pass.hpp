#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/lighting/cascade_shadow_mapping.hpp>
#include <limitless/renderer/instance_renderer.hpp>

namespace Limitless {
    /**
     * DirectionalShadowPass renders all objects and effects that casts shadow in shadow map
     */
    class DirectionalShadowPass final : public RendererPass {
    private:
        /**
         * CSM implementation (Filament-style)
         */
        CascadeShadowMapping shadows;
    public:
        explicit DirectionalShadowPass(Renderer& renderer);

        void update(const RendererSettings& settings) override;

        /**
         * Adds shadow-specific uniforms to setter
         */
        void addUniformSetter(UniformSetter& setter) override;

        /**
         * Draws shadow
         */
        void render(InstanceRenderer &renderer, Scene& scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;

        // Debug/introspection accessors (used by samples).
        [[nodiscard]] const CascadeShadowMapping& getShadows() const noexcept { return shadows; }
        [[nodiscard]] CascadeShadowMapping& getShadows() noexcept { return shadows; }
    };
}
