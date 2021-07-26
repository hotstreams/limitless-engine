#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    class DeferredLightingPass final : public RenderPass {
    public:
        explicit DeferredLightingPass(Pipeline& pipeline);
        ~DeferredLightingPass() override = default;

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}
