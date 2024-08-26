#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <limitless/renderer/instance_renderer.hpp>

namespace Limitless {
	class Framebuffer;

    /**
     * ScreenPass renders image to default framebuffer or the one specified
     *
     * note: as an input image uses previous result
     * note2: handling framebuffer resize is on user
     */
    class ScreenPass final : public PipelinePass {
    private:
        /**
         * Render target
         */
    	RenderTarget* target {};
    public:
        /**
         * Initializes pass to render to default framebuffer
         */
        explicit ScreenPass(Pipeline& pipeline);

        /**
         * Initializes pass to render to the specified render target
         */
        ScreenPass(Pipeline& pipeline, RenderTarget& target);

        /**
         * Sets render target in which to render
         */
        void setTarget(RenderTarget& target);

        /**
         * Renders image to render target
         */
        void
        draw(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera,
             UniformSetter &setter) override;
    };
}