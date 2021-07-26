#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    class CompositePass final : public RenderPass {
    private:
        Framebuffer framebuffer;
    public:
        explicit CompositePass(Pipeline& pipeline, ContextEventObserver& ctx);
        ~CompositePass() override = default;

        auto& getResult() { return framebuffer.get(FramebufferAttachment::Color0).texture; }

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}