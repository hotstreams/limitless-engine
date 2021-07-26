#pragma once

#include <limitless/pipeline/render_pass.hpp>

namespace Limitless {
    class QuadPass final : public RenderPass {
    public:
        explicit QuadPass(Pipeline& pipeline);
        ~QuadPass() override = default;

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}