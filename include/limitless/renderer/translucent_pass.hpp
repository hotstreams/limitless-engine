#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless::fx {
    class EffectRenderer;
}

namespace Limitless {
    /**
     * TranslucentPass renders translucent objects on-top of lighting result
     *
     *
     * Deferred renderer does not support translucent objects, so we have to render it with forward approach
     *
     * With this pass are renderer most effects (because they are transparent)
     *
     * Also this pass allows us to make screen space refraction because we have background image
     */
    class TranslucentPass final : public RendererPass {
    private:
        Framebuffer framebuffer;

        /**
         * The opaque scene's depth, copied before translucent objects are drawn. Translucent materials sample it as
         * "scene_depth_texture" (e.g. water depth and refraction), while those with depth_write write their own depth
         * into the shared depth buffer, which a texture cannot be sampled and written at once.
         */
        std::shared_ptr<Texture> scene_depth_copy;

        void copySceneDepth();
    public:
        explicit TranslucentPass(Renderer& renderer);

        std::shared_ptr<Texture> getResult();

        /**
         * Copies lighting result to result framebuffer and renders transparent objects for all blending states on top of it
         */
        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;

        /**
         * Updates framebuffer size
         */
        void onFramebufferChange(glm::uvec2 size) override;
    };
}