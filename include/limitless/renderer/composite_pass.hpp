#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/core/framebuffer.hpp>
#include <limitless/renderer/instance_renderer.hpp>

namespace Limitless {
    class CompositePass : public RendererPass {
    public:
        float tone_mapping_exposure = 1.0f;
        float gamma = 2.2f;

        explicit CompositePass(Renderer& renderer);

        std::shared_ptr<Texture> getResult();

        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;

        /**
         * Updates framebuffer size
         */
        void onFramebufferChange(glm::uvec2 size) override;

        ~CompositePass() override = default;

    protected:
        /**
         * Result framebuffer
         */
        Framebuffer framebuffer;
    };

    /**
     * CompositeWithBloomPass combines results from previous passes together
     *
     * result from transparent pass (lighting image + transparent objects) combined with bloom pass result
     */
    class CompositeWithBloomPass final : public CompositePass {
    public:
        explicit CompositeWithBloomPass(Renderer& renderer);

        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;
    };
}