#pragma once

#include <limitless/core/framebuffer.hpp>
#include <array>
#include <limitless/assets.hpp>

namespace Limitless {
    class Bloom {
    private:
        static constexpr uint8_t blur_iterations = 8;

        Framebuffer brightness;
        std::array<Framebuffer, 2> blur;

        void extractBrightness(const Assets& ctx, const std::shared_ptr<Texture>& image);
        void blurImage(const Assets& ctx);
    public:
        explicit Bloom(ContextEventObserver& ctx);

        void process(const Assets& ctx, const std::shared_ptr<Texture>& image);

        [[nodiscard]] const std::shared_ptr<Texture>& getResult() const noexcept;
    };

    class PostProcessing final {
    public:
        bool bloom {true};

        bool tone_mapping {true};
        float exposure {1.0f};

        bool gamma_correction {};
        float gamma {2.2f};

        bool vignette {};
        float vignette_radius {0.55f};
        float vignette_softness {0.15f};

        bool tone_shading {};
        float number_of_colors {7.7f};
        float line_texel_offset {5.0f};
        float line_multiplier {1000.0f};
        float line_bias {2.0f};

        bool fxaa {true};

        RenderTarget& target;
    private:
        Bloom bloom_process;
    public:
        explicit PostProcessing(ContextEventObserver& ctx, RenderTarget& target = default_framebuffer);
        ~PostProcessing() = default;

        void process(Context& context, const Assets& assets, const Framebuffer& offscreen);
    };
}
