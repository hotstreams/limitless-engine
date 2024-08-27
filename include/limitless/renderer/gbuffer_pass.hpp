#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
    /**
     * GBufferPass renders opaque objects to DeferredFramebufferPass's framebuffer to populate material data
     * used for later light calculations
     */
    class GBufferPass final : public RendererPass {
    public:
        explicit GBufferPass(Renderer& renderer);

        /**
         * Fills GBUFFER with opaque objects and effects data
         */
        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;
    };
}