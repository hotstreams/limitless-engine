#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/pipeline/postprocessing.hpp>

namespace Limitless {
    class PostEffectsPass final : public RenderPass {
    private:
        PostProcessing postprocess;
    public:
        PostEffectsPass(RenderPass* prev, ContextEventObserver& context);
        PostEffectsPass(RenderPass* prev, ContextEventObserver& context, RenderTarget& target);
        ~PostEffectsPass() override = default;

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, const UniformSetter& setter) override;
    };
}