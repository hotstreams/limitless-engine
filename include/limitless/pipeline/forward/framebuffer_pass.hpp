#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    class FramebufferPass final : public PipelinePass {
    private:
        Framebuffer framebuffer;
        RenderTarget& target;
    public:
        FramebufferPass(Pipeline& pipeline, RenderTarget& _target);
        FramebufferPass(Pipeline& pipeline, Context& ctx);
        ~FramebufferPass() override = default;

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}