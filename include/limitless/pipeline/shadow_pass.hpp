#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/lighting/cascade_shadows.hpp>

namespace Limitless {
    class DirectionalShadowPass final : public RenderPass {
    private:
        CascadeShadows shadows;
        std::reference_wrapper<DirectionalLight> light;
    public:
        DirectionalShadowPass(RenderPass* prev, Context& ctx, Scene& scene);
        ~DirectionalShadowPass() override = default;

        void addSetter(UniformSetter& setter) override;
        void update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera) override;
        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, const UniformSetter& setter) override;
    };
}