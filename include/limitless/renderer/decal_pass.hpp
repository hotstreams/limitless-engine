#pragma once

#include <limitless/renderer/renderer_pass.hpp>

namespace Limitless {
    class DecalPass final : public RendererPass {
    public:
        explicit DecalPass(Renderer& renderer);

        /**
         * Fills GBUFFER with opaque objects and effects data
         */
        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;
    };
}