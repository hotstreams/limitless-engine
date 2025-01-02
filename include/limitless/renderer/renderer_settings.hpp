#pragma once

#include <limitless/renderer/shader_type.hpp>
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
    class RendererSettings {
    public:
        /**
         *  Normal mapping
         */
        bool normal_mapping {true};

        /**
         * Screen Space Ambient Occlusion
         */
        bool screen_space_ambient_occlusion {false};
        SSAO::Settings ssao_settings;

        /**
         * Screen Space Reflections
         */
        bool screen_space_reflections {false};
        SSR::Settings ssr_settings;

        /**
         * FXAA Fast Approximate Antialiasing
         */
        bool fast_approximate_antialiasing {false};
        //TODO: refactor shaders

        /**
         * Cascade shadow maps
         */
        bool cascade_shadow_maps = false;
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
        bool coordinate_system_axes = true;

        /**
         * Render bounding boxes
         */
        bool bounding_box = true;

        class Builder {
        private:
            /**
             *  Normal mapping
             */
            bool normal_mapping {true};

            /**
             * Screen Space Ambient Occlusion
             */
            bool screen_space_ambient_occlusion {false};
            SSAO::Settings ssao_cfg;

            /**
             * Screen Space Reflections
             */
            bool screen_space_reflections {false};
            SSR::Settings ssr_cfg;

            /**
             * FXAA Fast Approximate Antialiasing
             */
            bool fast_approximate_antialiasing {false};

            /**
             * Cascade shadow maps
             */
            bool cascade_shadow_maps = false;
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
            float bloom_ex_threshold {1.0f};
            float bloom_str {1.0f};
            uint32_t bloom_blur_it_count {8};

            /**
             *
             */
            bool specular_aa {true};
            float specular_threshold {0.1f};
            float specular_variance {0.2f};

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
            bool coordinate_system_axes = true;

            /**
             * Render bounding boxes
             */
            bool bounding_box = true;
        public:
            Builder& enable_normal_mapping();
            Builder& disable_normal_mapping();

            Builder& enable_ssao();
            Builder& disable_ssao();
            Builder& ssao_settings(SSAO::Settings settings);
            Builder& ssao_sample_count(glm::vec2 count);
            Builder& ssao_spiral_turns(float turns);
            Builder& ssao_power(float power);
            Builder& ssao_bias(float bias);

            Builder& enable_ssr();
            Builder& disable_ssr();
            Builder& ssr_settings(SSR::Settings settings);

            Builder& enable_fxaa();
            Builder& disable_fxaa();

            Builder& enable_csm();
            Builder& disable_csm();
            Builder& csm_texture_resolution(glm::uvec2 resolution);
            Builder& csm_count(uint32_t count);
            Builder& csm_enable_pcf();
            Builder& csm_disable_pcf();
            Builder& csm_enable_micro_shadowing();
            Builder& csm_disable_micro_shadowing();

            Builder& enable_bloom();
            Builder& disable_bloom();
            Builder& bloom_extract_threshold(float threshold);
            Builder& bloom_strength(float strength);
            Builder& bloom_blur_iteration_count(uint32_t count);

            Builder& enable_specular_aa();
            Builder& disable_specular_aa();
            Builder& specular_aa_threshold(float threshold);
            Builder& specular_aa_variance(float variance);

            Builder& debug_light_radius();
            Builder& debug_coordinate_system_axes();
            Builder& debug_bounding_box();

            RendererSettings build();
        };

        static Builder builder() { return {}; }
    };
}
