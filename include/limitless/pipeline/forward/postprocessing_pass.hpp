#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <limitless/pipeline/forward/postprocessing.hpp>

namespace Limitless {
    class PostEffectsPass final : public PipelinePass {
    private:
        PostProcessing postprocess;
    public:
        PostEffectsPass(Pipeline& pipeline, Context& context);
        PostEffectsPass(Pipeline& pipeline, Context& context, RenderTarget& target);
        ~PostEffectsPass() override = default;

        void
        draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera,
             UniformSetter &setter) override;
    };
}