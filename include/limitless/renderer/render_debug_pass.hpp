#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/util/renderer_helper.hpp>
#include <limitless/lighting/lighting.hpp>

namespace Limitless {
    class RenderDebugPass final : public RendererPass {
    private:
        RendererHelper helper;
    public:
        explicit RenderDebugPass(Renderer& renderer);

        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;
    };
}