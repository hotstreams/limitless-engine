#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/core/framebuffer.hpp>
#include <limitless/renderer/instance_renderer.hpp>

namespace Limitless {
    /**
     * CompositePass combines results from previous passes together
     *
     * result from transparent pass (lighting image + transparent objects) combined with bloom pass result
     */
    class CompositePass final : public RendererPass {
    private:
        /**
         * Result framebuffer
         */
        Framebuffer framebuffer;
    public:
        float tone_mapping_exposure = 1.0f;

        explicit CompositePass(Renderer& renderer);

        std::shared_ptr<Texture> getResult();

        /**
         *
         */
        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;

        /**
         * Updates framebuffer size
         */
        void onFramebufferChange(glm::uvec2 size) override;
    };
}