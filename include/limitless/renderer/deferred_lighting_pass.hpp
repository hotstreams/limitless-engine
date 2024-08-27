#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/core/framebuffer.hpp>
#include <limitless/renderer/instance_renderer.hpp>

namespace Limitless {
    /**
     * DeferredLightingPass makes lighting calculation using data from GBUFFER and renders result to its own framebuffer
     */
    class DeferredLightingPass final : public RendererPass {
    private:
        /**
         * Lighting result framebuffer
         */
        Framebuffer framebuffer;
    public:
        explicit DeferredLightingPass(Renderer& renderer);

        auto& getFramebuffer() noexcept { return framebuffer; }

        std::shared_ptr<Texture> getResult();

        /**
         * Renders lighting to framebuffer
         */
        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;

        /**
         * Updates framebuffer size
         */
        void onFramebufferChange(glm::uvec2 size) override;
    };
}
