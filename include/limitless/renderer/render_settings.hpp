#pragma once

#include <limitless/pipeline/shader_type.hpp>
#include <glm/vec2.hpp>
#include <limitless/postprocessing/ssr.hpp>
#include <limitless/postprocessing/ssao.hpp>

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
        SSAO::Settings settings;

        /**
         * Screen Space Reflections
         */
        bool screen_space_reflections {false};
        SSR::Settings screen_space_reflections_settings;

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
        bool csm_micro_shadowing = true;

        /**
         *
         */
        bool bloom {true};
        float bloom_extract_threshold {1.0f};
        float bloom_strength {1.0f};
        uint32_t bloom_blur_iteration_count {8};

        /**
         *
         */
         bool specular_aa {true};
         float specular_aa_threshold {0.1f};
         float specular_aa_variance {0.2f};

        /**
         * Debug settings
         */

        /**
         *  Render lights impact area
         */
        bool light_radius = false;

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
