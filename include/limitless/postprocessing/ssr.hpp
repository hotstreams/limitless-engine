#pragma once

#include <limitless/core/framebuffer.hpp>
#include <limitless/postprocessing/blur.hpp>

namespace Limitless {
    class UniformSetter;
    class Assets;
    class Camera;
    class Renderer;

    class SSR {
    public:
        class Settings {
        public:
            float vs_thickness = 0.5f;
            float stride = 2.0f;
            float vs_max_distance = 1000.0f;
            float bias = 0.1f;
            float max_steps = 32.0f;
            float reflection_threshold = 0.0f;
            float roughness_factor = 0.1f;
            float reflection_strength = 5.0f;
            float reflection_falloff_exp = 1.0f;

            bool intersection_distance_attenuation = true;
            bool iteration_count_attenuation = true;
            bool borders_attenuation = true;
            bool fresnel_attenuation = true;

            bool camera_facing_attenuation = true;
            float camera_attenuation_lower_edge = 0.2f;
            float camera_attenuation_upper_edge = 0.55f;

            bool clip_to_frustrum = true;
            bool refiniment = true;

            bool reflection_blur = true;
            uint8_t blur_iteration_count = 4;
            float blur_strength = 1.0f;
        private:
//            float reflections_strength;
        };
    private:
        Framebuffer framebuffer;
        Settings settings;
        Blur blur;
    public:
        SSR(Renderer& renderer);

        std::shared_ptr<Texture> getResult();

        void update();

        void addSetter(UniformSetter& setter);

        void draw(Context& ctx,
                  const Assets& assets,
                  const Camera& camera,
                  const std::shared_ptr<Texture>& depth,
                  const std::shared_ptr<Texture>& normal,
                  const std::shared_ptr<Texture>& props,
                  const std::shared_ptr<Texture>& image);

        void onFramebufferChange(glm::uvec2 frame_size);
    };
}