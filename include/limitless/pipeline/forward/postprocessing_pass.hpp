#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/pipeline/forward/postprocessing.hpp>

namespace Limitless {
    class PostEffectsPass final : public RenderPass {
    private:
        PostProcessing postprocess;
    public:
        PostEffectsPass(Pipeline& pipeline, ContextEventObserver& context);
        PostEffectsPass(Pipeline& pipeline, ContextEventObserver& context, RenderTarget& target);
        ~PostEffectsPass() override = default;

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}