#pragma once

#include <limitless/pipeline/render_pass.hpp>

namespace Limitless {
	class Framebuffer;

    class FinalQuadPass final : public RenderPass {
    private:
    	RenderTarget* target {};
    public:
        explicit FinalQuadPass(Pipeline& pipeline);
        FinalQuadPass(Pipeline& pipeline, RenderTarget& target);

        void setTarget(RenderTarget& target);

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}