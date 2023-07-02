#pragma once

#include <limitless/pipeline/render_pass.hpp>
#include <limitless/core/framebuffer.hpp>
#include <limitless/renderer/render_settings.hpp>

namespace Limitless {
    /**
     * FXAAPass implements Fast Approximate Antialiasing
     *
     * note: as an input image gets previous result
     */
    class FXAAPass final : public RenderPass {
    private:
        /**
         * Result framebuffer
         */
    	Framebuffer framebuffer;
    public:
        explicit FXAAPass(Pipeline& pipeline, glm::uvec2 frame_size);

        std::shared_ptr<Texture> getResult() override;

        /**
         *  Applies FXAA
         */
	    void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;

        /**
         * Updates framebuffer size
         */
        void onFramebufferChange(glm::uvec2 size) override;
    };
}