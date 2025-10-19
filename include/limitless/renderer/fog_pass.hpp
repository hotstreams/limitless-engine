#pragma once

#include <limitless/renderer/renderer_pass.hpp>
#include <limitless/core/context.hpp>
#include <limitless/core/framebuffer.hpp>

namespace Limitless {
	class FogPass : public RendererPass {
    private:
        Framebuffer framebuffer;
	public:
        // Fog parameters
        glm::vec3 fog_color = glm::vec3(0.75f, 0.75f, 0.75f);
        float fog_density = 0.01f;
        float fog_start = 10.0f;
        float fog_end = 20.0f;
        float fog_height_start = 5.0f;   // altitude where attenuation begins
        float fog_height_end = 9.0f;    // altitude where fog fully attenuates
        bool fog_enabled = true;

		explicit FogPass(Renderer& renderer);

        std::shared_ptr<Texture> getResult();

        void render(InstanceRenderer &renderer, Scene &scene, Context &ctx, const Assets &assets, const Camera &camera, UniformSetter &setter) override;

        void onFramebufferChange(glm::uvec2 size) override;
    };
}
