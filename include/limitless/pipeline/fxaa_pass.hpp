#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/core/framebuffer.hpp>
#include <limitless/pipeline/render_settings.hpp>

namespace Limitless {
    class FXAAPass final : public RenderPass {
    private:
    	Framebuffer framebuffer;
    public:
        explicit FXAAPass(Pipeline& pipeline, glm::uvec2 frame_size);

        std::shared_ptr<Texture> getResult() override { return framebuffer.get(FramebufferAttachment::Color0).texture; }

	    void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;

        void onFramebufferChange(glm::uvec2 size) override;
    };
}