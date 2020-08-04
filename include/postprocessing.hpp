#pragma once

#include <framebuffer.hpp>

namespace GraphicsEngine {
    class Bloom {
    private:
        static constexpr uint8_t blur_iterations = 8;

        Framebuffer brightness;
        std::array<Framebuffer, 2> blur;

        void extractBrightness(const std::shared_ptr<Texture>& image);
        void blurImage();
    public:
        explicit Bloom(ContextEventObserver& ctx);

        void process(const std::shared_ptr<Texture>& image);

        const std::shared_ptr<Texture>& getResult() const noexcept;
    };

    class PostProcessing {
    public:
        bool bloom {true};

        bool tone_mapping {true};
        float exposure {1.0f};

        bool gamma_correction {false};
        float gamma {2.2f};

        bool vignette {true};
        float vignette_radius {0.75f};
        float vignette_softness {0.45f};

        bool tone_shading {true};
        float number_of_colors {7.7f};
        float line_texel_offset {5.0f};
        float line_multiplier {1000.0f};
        float line_bias {2.0f};
    private:
        Bloom bloom_process;
    public:
        explicit PostProcessing(ContextEventObserver& ctx);

        void process(Context& context, const Framebuffer& offscreen);
    };
}
