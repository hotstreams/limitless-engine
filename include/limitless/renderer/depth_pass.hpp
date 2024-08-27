#pragma once

#include <limitless/renderer/renderer_pass.hpp>

namespace Limitless {
    class Renderer;
    /**
     * Depth pre-pass to render only to depth buffer from DeferredFramebufferPass, fragments color gets discarded
     *
     * so we can discard useless fragments later and restore positions from depth for postprocessing effects
     */
    class DepthPass final : public RendererPass {
    public:
        explicit DepthPass(Renderer& renderer);

        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;
    };
}