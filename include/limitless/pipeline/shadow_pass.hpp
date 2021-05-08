#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/lighting/cascade_shadows.hpp>

namespace Limitless::fx {
    class EffectRenderer;
}

namespace Limitless {
    class DirectionalShadowPass final : public RenderPass {
    private:
        std::optional<std::reference_wrapper<fx::EffectRenderer>> effect_renderer;
        CascadeShadows shadows;
        std::reference_wrapper<DirectionalLight> light;
    public:
        DirectionalShadowPass(RenderPass* prev, Context& ctx, Scene& scene, const RenderSettings& settings);
        DirectionalShadowPass(RenderPass* prev, Context& ctx, Scene& scene, const RenderSettings& settings, fx::EffectRenderer& renderer);
        ~DirectionalShadowPass() override = default;

        void addSetter(UniformSetter& setter) override;
        void update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera) override;
        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, const UniformSetter& setter) override;
    };
}