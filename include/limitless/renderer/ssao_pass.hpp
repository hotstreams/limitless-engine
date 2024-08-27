#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/core/framebuffer.hpp>
#include <limitless/postprocessing/ssao.hpp>
#include <limitless/renderer/instance_renderer.hpp>

namespace Limitless {
    class SSAOPass final : public RendererPass {
    private:
        SSAO ssao;
    public:
        explicit SSAOPass(Renderer& renderer);

        std::shared_ptr<Texture> getResult() { return ssao.getFramebuffer().get(FramebufferAttachment::Color0).texture; }

        void addUniformSetter(UniformSetter &setter) override;

        void update(Scene &scene, const Camera &camera) override;

        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;

        void onFramebufferChange(glm::uvec2 size) override;
    };
}
