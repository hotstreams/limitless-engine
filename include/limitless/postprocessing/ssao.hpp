#pragma once

#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <limitless/core/framebuffer.hpp>
#include <limitless/scene.hpp>

namespace Limitless {
    class Assets;
    class Renderer;

    /**
     * Filament `AmbientOcclusionOptions::quality` (see filament PostProcessManager::screenSpaceAmbientOcclusion).
     */
    enum class SsaoFilamentQuality : uint8_t {
        Low = 0,
        Medium = 1,
        High = 2,
        Ultra = 3
    };

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
            glm::vec2 sample_count {7.0f, 1.0f / (7.0f - 0.5f)};
            glm::vec2 angle_inc_cos_sin {0.0f, 0.0f};
            glm::vec4 ssao_resolution {0.0f};
            float projection_scale_radius {0.0f};
            float intensity {0.0f};
            float spiral_turns {14.0f};
            float inv_radius_squared {0.0f};
            float min_horizon_angle_sine_squared {0.0f};
            float bias {0.0005f};
            float peak2 {0.0f};
            /// User exponent; uploaded to GPU as `power * 2` (Filament SAO convention).
            float power {1.0f};
            /// Minimum horizon angle (radians); default 0 matches Filament.
            float min_horizon_angle_rad {0.0f};
            uint32_t max_level {7};
            uint32_t debug_mode {0};

            /** Sample/spiral/power/bias/min_horizon match Filament for the given quality tier. */
            static Settings filamentQualityPreset(SsaoFilamentQuality q) noexcept;
        };
    private:
        Framebuffer framebuffer;
        std::shared_ptr<Buffer> buffer;
        Settings settings;
        glm::uvec2 frame_resolution {1080, 720};

        void updateSettings(const Camera& camera, const Settings& user);
    public:
        explicit SSAO(Renderer& renderer);

        const auto& getFramebuffer() const noexcept { return framebuffer; }

        void draw(Context &ctx, const Assets &assets, const std::shared_ptr<Texture>& depth, const std::shared_ptr<Texture>& normal);

        void update(const Camera& camera, const Settings& user);

        void onFramebufferChange(glm::uvec2 frame_size);
    };
}
