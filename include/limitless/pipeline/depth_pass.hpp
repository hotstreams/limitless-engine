#pragma once

#include <limitless/pipeline/render_pass.hpp>

namespace Limitless::fx {
    class EffectRenderer;
}

namespace Limitless {
    class DepthPass final : public RenderPass {
    private:
        fx::EffectRenderer& renderer;
    public:
        DepthPass(Pipeline& pipeline, fx::EffectRenderer& renderer);
        ~DepthPass() override = default;

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}