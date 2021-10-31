#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/core/framebuffer.hpp>
#include <limitless/pipeline/render_settings.hpp>

namespace Limitless {
    class FXAAPass final : public RenderPass {
    private:
    	Framebuffer framebuffer;
	    RenderTarget& target;
	    void initialize(Context& ctx);
    public:
        explicit FXAAPass(Pipeline& pipeline, Context& ctx);
        explicit FXAAPass(Pipeline& pipeline, RenderTarget& target);
        ~FXAAPass() override = default;

        std::shared_ptr<Texture> getResult() override { return framebuffer.get(FramebufferAttachment::Color0).texture; }

	    void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;
    };
}