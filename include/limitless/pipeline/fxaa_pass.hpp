#pragma once

#include <limitless/pipeline/render_pass.hpp>

namespace Limitless {
    class FXAAPass final : public RenderPass {
    private:
	    RenderTarget& target;
    public:
        explicit FXAAPass(Pipeline& pipeline);
        explicit FXAAPass(Pipeline& pipeline, RenderTarget& target);
        ~FXAAPass() override = default;

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}