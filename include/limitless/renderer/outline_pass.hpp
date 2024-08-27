#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/core/context.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
	class OutlinePass : public RendererPass {
    private:
        Framebuffer framebuffer;
	public:
        glm::vec3 outline_color = glm::vec3(1.0, 0.0, 0.0);
        int32_t width = 2;

		explicit OutlinePass(Renderer& renderer);

        std::shared_ptr<Texture> getResult();

        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;

        void onFramebufferChange(glm::uvec2 size) override;
    };
}