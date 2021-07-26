#pragma once

#include <limitless/pipeline/render_pass.hpp>

namespace Limitless {
    class FXAAPass final : public RenderPass {
    public:
        explicit FXAAPass(Pipeline& pipeline);
        ~FXAAPass() override = default;

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}