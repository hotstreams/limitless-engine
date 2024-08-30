#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/core/framebuffer.hpp>
#include <limitless/postprocessing/ssr.hpp>
#include <limitless/renderer/instance_renderer.hpp>

namespace Limitless {
    class SSRPass final : public RendererPass {
    private:
        SSR ssr;
    public:
        SSRPass(Renderer& renderer);

        std::shared_ptr<Texture> getResult() { return ssr.getResult(); }

        void addUniformSetter(UniformSetter &setter) override;

//        void update(Limitless::Scene &scene, Limitless::Instances &instances, Limitless::Context &ctx, const Limitless::Camera &camera) override;

        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;

        void onFramebufferChange(glm::uvec2 size) override;
    };
}
