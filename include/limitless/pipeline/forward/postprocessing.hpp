#pragma once

#include <limitless/core/framebuffer.hpp>
#include <array>
#include <limitless/assets.hpp>
#include "limitless/postprocessing/bloom.hpp"

namespace Limitless {
    class PostProcessing final {
    public:
        bool bloom {true};

        bool tone_mapping {false};
        float exposure {1.0f};

        bool gamma_correction {true};
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
        explicit PostProcessing(glm::uvec2 frame_size, RenderTarget& target = default_framebuffer);

        void onResize(glm::uvec2 frame_size);

        void process(Context& context, const Assets& assets, const Framebuffer& offscreen);
    };
}
