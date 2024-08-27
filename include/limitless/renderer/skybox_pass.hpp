#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/renderer/instance_renderer.hpp>

namespace Limitless {

    /**
     * SkyboxPass renders skybox from scene
     *
     * note: uses current framebuffer
     */
    class SkyboxPass final : public RendererPass {
    private:
    public:
        explicit SkyboxPass(Renderer& renderer);

        /**
         * Renders skybox to current bound framebuffer
         */
        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;
    };
}