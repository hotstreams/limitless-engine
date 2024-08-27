#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/postprocessing/bloom.hpp>

namespace Limitless {
    /**
     * BloomPass implements Bloom for an image
     *
     * It extracts color values within some defined threshold and then applies Down-Up-sampling blur
     *
     * note: as an input image uses previous pass result
     */
    class BloomPass final : public RendererPass {
    private:
        /**
         * Bloom implementation
         */
        Bloom bloom;
    public:
        explicit BloomPass(Renderer& renderer);

        std::shared_ptr<Texture> getResult();
        auto& getBloom() noexcept { return bloom; }

        /**
         * Makes image bloom-ish
         */
        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;

        /**
         * Update framebuffer size
         */
        void onFramebufferChange(glm::uvec2 size) override;
    };
}