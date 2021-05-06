#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    class FramebufferPass final : public RenderPass {
    private:
        Framebuffer framebuffer;
        RenderTarget& target;
    public:
        FramebufferPass(RenderPass* prev, RenderTarget& _target);
        FramebufferPass(RenderPass* prev, Context& ctx);

        RenderTarget& getTarget() noexcept override;
        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, const UniformSetter& setter) override;
    };
}