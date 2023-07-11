#pragma once

#include <cstdint>
#include <glm/vec4.hpp>
#include <vector>
#include <limitless/core/framebuffer.hpp>
#include <limitless/scene.hpp>

namespace Limitless {
    class Assets;
    /**
     * Screen Space Ambient Occlusion
     */
    class SSAO {
    public:
        /**
         * SSAO settings
         */
        class Settings {
        public:
            glm::vec2 sample_count;
            glm::vec2 angle_inc_cos_sin;
            float projection_scale_radius;
            float intensity;
            float spiral_turns {14.0f};
            float inv_radius_squared;
            float min_horizon_angle_sine_squared;
            float bias;
            float peak2;
            float power;
            uint32_t max_level;
        };
    private:
        Framebuffer framebuffer;
        std::shared_ptr<Buffer> buffer;
        Settings settings;

        void updateSettings(const Camera& camera);
    public:
        SSAO(ContextEventObserver &ctx, glm::uvec2 frame_size);
        explicit SSAO(ContextEventObserver &ctx);

        const auto& getFramebuffer() const noexcept { return framebuffer; }

        void draw(Context &ctx, const Assets &assets, const std::shared_ptr<Texture>& depth);

        void update(const Camera& camera);

        void onFramebufferChange(glm::uvec2 frame_size);
    };
}
