#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless::fx {
    class EffectRenderer;
}

namespace Limitless {
    class GBufferPass final : public RenderPass {
    private:
        fx::EffectRenderer& renderer;
    public:
        GBufferPass(Pipeline& pipeline, fx::EffectRenderer& renderer);
        ~GBufferPass() override = default;

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}