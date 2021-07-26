#pragma once

#include <limitless/pipeline/render_pass.hpp>

namespace Limitless::fx {
    class EffectRenderer;
}

namespace Limitless::ms {
    enum class Blending;
}

namespace Limitless {
    class ParticlePass final : public RenderPass {
    private:
        std::reference_wrapper<fx::EffectRenderer> renderer;
        ms::Blending blending;
    public:
        ParticlePass(Pipeline& pipeline, fx::EffectRenderer& renderer, ms::Blending blending);
        ~ParticlePass() override = default;

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}