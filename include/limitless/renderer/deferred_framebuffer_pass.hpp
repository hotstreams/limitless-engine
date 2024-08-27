#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/core/framebuffer.hpp>
#include <limitless/renderer/instance_renderer.hpp>

namespace Limitless {
    class DeferredFramebufferPass final : public RendererPass {
    private:
        Framebuffer framebuffer;
    public:
        DeferredFramebufferPass(Renderer& renderer);

        /**
         * Gets GBUFFER textures
         */
        auto& getFramebuffer() noexcept { return framebuffer; }
        auto& getAlbedo() noexcept { return framebuffer.get(FramebufferAttachment::Color0).texture; }
        auto& getNormal() noexcept { return framebuffer.get(FramebufferAttachment::Color1).texture; }
        auto& getProperties() noexcept { return framebuffer.get(FramebufferAttachment::Color2).texture; }
        auto& getInfo() noexcept { return framebuffer.get(FramebufferAttachment::Color4).texture; }
        auto& getEmissive() noexcept { return framebuffer.get(FramebufferAttachment::Color3).texture; }
        auto& getDepth() noexcept { return framebuffer.get(FramebufferAttachment::Depth).texture; }

        /**
         * Sets up context state, binds framebuffer
         */
        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;

        /**
         * Updates framebuffer size
         */
        void onFramebufferChange(glm::uvec2 size) override;
    };
}