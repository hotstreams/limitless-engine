#pragma once

#include <limitless/pipeline/render_pass.hpp>

namespace Limitless {
	class Framebuffer;

    class QuadPass final : public RenderPass {
    private:
    	RenderTarget* target;
    public:
        explicit QuadPass(Pipeline& pipeline);
        QuadPass(Pipeline& pipeline, RenderTarget& target);
        ~QuadPass() override = default;

        void setTarget(RenderTarget* target);

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}