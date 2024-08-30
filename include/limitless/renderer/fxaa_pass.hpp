#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/core/framebuffer.hpp>
#include <limitless/renderer/renderer_settings.hpp>

namespace Limitless {
    /**
     * FXAAPass implements Fast Approximate Antialiasing
     *
     * note: as an input image gets previous result
     */
    class FXAAPass final : public RendererPass {
    private:
        /**
         * Result framebuffer
         */
    	Framebuffer framebuffer;
    public:
        explicit FXAAPass(Renderer& renderer);

        std::shared_ptr<Texture> getResult();

        /**
         *  Applies FXAA
         */
        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;

        /**
         * Updates framebuffer size
         */
        void onFramebufferChange(glm::uvec2 size) override;
    };
}