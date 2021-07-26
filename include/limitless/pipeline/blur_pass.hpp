#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/pipeline/postprocessing.hpp>

namespace Limitless::fx {
    class EffectRenderer;
}

namespace Limitless {
    class BlurPass final : public RenderPass {
    private:
        Blur blur;
    public:
        explicit BlurPass(Pipeline& pipeline, ContextEventObserver& ctx);
        ~BlurPass() override = default;

        auto& getBlurResult() { return blur.getResult(); }

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}