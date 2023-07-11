#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <limitless/fx/effect_renderer.hpp>

namespace Limitless {
    class EffectUpdatePass final : public PipelinePass {
    private:
        fx::EffectRenderer renderer;
    public:
        EffectUpdatePass(Pipeline& pipeline, Context& ctx);
        ~EffectUpdatePass() override = default;

        auto& getRenderer() { return renderer; }

        void update(Scene& scene, Instances& instances, Context& ctx, const Camera& camera) override;
    };
}