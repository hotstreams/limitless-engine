#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/ms/blending.hpp>

namespace Limitless::fx {
    class EffectRenderer;
}

namespace Limitless {
    class TranslucentPass final : public RenderPass {
    private:
        fx::EffectRenderer& renderer;

        void sort(Instances& instances, const Camera& camera, ms::Blending blending);
    public:
        explicit TranslucentPass(Pipeline& pipeline, fx::EffectRenderer& renderer);

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}