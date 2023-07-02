#pragma once

#include <limitless/pipeline/shader_pass_types.hpp>
#include <glm/vec2.hpp>

namespace Limitless {
    enum class RenderPipeline {
        Forward,
        Deferred
    };

    /**
     *
     */
    class RenderSettings {
    public:
        RenderPipeline pipeline = RenderPipeline::Deferred;

        /**
         *  Normal mapping
         */
        bool normal_mapping {true};

        /**
         * Screen Space Ambient Occlusion
         */
        bool screen_space_ambient_occlusion {false};
        uint32_t ssao_noise_size {4u};
        uint32_t ssao_kernel_samples_count {64u};
        float ssao_radius {3.0f};
        float ssao_bias {2e-3f};
        //TODO: check with shaders

        /**
         * FXAA Fast Approximate Antialiasing
         */
        bool fast_approximate_antialiasing {false};
        //TODO: refactor shaders

        /**
         * Bokeh Depth of field
         */
        bool depth_of_field = false;
        glm::vec2 dof_focus {0.7f, 0.5f};
        glm::vec2 dof_distance {0.3f, 0.55f};
        //TODO: check with shaders

        /**
         * Cascade shadow maps
         */
        bool cascade_shadow_maps = true;
        glm::uvec2 csm_resolution = {1024 * 4, 1024 * 4 };
        uint8_t csm_split_count = 3; // [2; 4]

        /**
         * Percentage-Closer Filtering for CSM
         */
        bool csm_pcf = true;

        /**
         * Micro-shadowing for CSM
         */
        bool csm_micro_shadowing = false;

        /**
         *
         */
        bool bloom {true};
        float bloom_extract_threshold {1.0f};
        float bloom_strength {1.0f};
        uint32_t bloom_blur_iteration_count {8};

        /**
         * Debug settings
         */

        /**
         *  Render lights impact area
         */
        bool light_radius = true;

        /**
         * Render System Axes
         */
        bool coordinate_system_axes = false;

        /**
         * Render bounding boxes
         */
        bool bounding_box = false;
    };
}
