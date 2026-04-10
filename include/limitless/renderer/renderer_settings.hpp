#pragma once

#include <limitless/renderer/shader_type.hpp>
#include <glm/vec2.hpp>
#include <limitless/postprocessing/ssr.hpp>
#include <limitless/postprocessing/ssao.hpp>
#include <glm/vec3.hpp>
#include <optional>
#include <string>

namespace Limitless {
    enum class RenderPipeline {
        Forward,
        Deferred
    };

    /**
     * Screen-space ambient occlusion implementation for deferred (mutually exclusive pass).
     */
    enum class AmbientOcclusionMode : uint8_t {
        None = 0,
        SAO = 1,
        HBAO = 2
    };

    /**
     * Compile-time FXAA quality (NVIDIA FXAA_QUALITY__PRESET in shaders/functions/fxaa.glsl).
     * Changing this requires shader recompilation.
     */
    enum class FxaaQualityPreset : int {
        Low = 12,
        Medium = 23,
        High = 29,
        Ultra = 39
    };

    /**
     * Runtime FXAA (NVIDIA FXAA 3.11 quality) tuning; used by FXAAPass.
     */
    struct FxaaSettings {
        FxaaQualityPreset quality {FxaaQualityPreset::High};
        float subpix {0.75f};
        float edge_threshold {0.166f};
        float edge_threshold_min {0.0833f};
    };

    /**
     *
     */
    class RendererSettings {
    public:
        enum class WindQuality : uint8_t {
            None = 0,
            Fastest = 1,
            Fast = 2,
            Good = 3,
            Best = 4
        };

        enum class Tonemapper : uint8_t {
            Exponential = 0,
            ACES = 1,
            Filmic = 2
        };

        struct PostProcessing {
            // Tonemapping
            Tonemapper tonemapper {Tonemapper::Exponential};
            float exposure {1.0f};
            float gamma {2.2f}; // display gamma

            // Basic color grading (applied after tonemapping, before display gamma)
            float white_balance_temperature {0.0f}; // [-1..1] (warm/cool)
            float white_balance_tint {0.0f};        // [-1..1] (green/magenta)

            float contrast {1.0f};   // 1.0 = neutral
            float saturation {1.0f}; // 1.0 = neutral

            // Lift/Gamma/Gain grade (approx. shadows/mids/highlights)
            glm::vec3 lift {0.0f};
            glm::vec3 grade_gamma {1.0f};
            glm::vec3 gain {1.0f};

            // 3D LUT (applied in composite pass, after tonemap/grading, before display gamma)
            bool lut_enabled {false};
            std::string lut_texture {}; // lookup name in Assets.textures
            float lut_intensity {1.0f}; // 0..1

            /**
             * Compile-time composite pass features (single shader variant per Assets compile).
             * Changing any of these requires Assets::recompileAssets.
             */
            bool composite_white_balance {true};
            bool composite_color_grading {true};
            bool composite_display_gamma {true};
        } postprocess;

        struct HeightFog {
            /**
             * When true, composite.frag is compiled with height fog (depth sample, reconstruct).
             * Runtime visibility uses `enabled` without recompilation.
             */
            bool include_in_composite_shader {false};
            bool enabled {false};
            glm::vec3 color {0.6f, 0.7f, 0.8f};
            // Base density. Higher = more fog.
            float density {0.02f};
            // World-space height where fog density is defined.
            float height {0.0f};
            // Density falloff with height: density *= exp(-falloff * (y - height)).
            float falloff {0.15f};
            // No fog applied for distances < start_distance.
            float start_distance {0.0f};
        } height_fog;

        /**
         * Wind quality tier (compile-time in shaders via defines).
         *
         * NOTE: changing this requires shader recompilation.
         */
        WindQuality wind_quality {WindQuality::Good};
        /**
         *  Normal mapping
         */
        bool normal_mapping {true};

        /**
         * Screen-space ambient occlusion: off, SAO (half-res spiral), or HBAO (full-res, NVIDIA gl_ssao classic).
         */
        AmbientOcclusionMode ambient_occlusion_mode {AmbientOcclusionMode::None};
        SSAO::Settings ssao_settings;
        /// Set when built with `Builder::ssao_filament_quality`; unset after manual `ssao_settings`.
        std::optional<SsaoFilamentQuality> ssao_filament_quality_preset;

        /// Runtime tuning for HBAO (NVIDIA sample–style horizon AO, not HBAO+ / ShadowWorks).
        struct HbaoSettings {
            float radius {2.0f};
            float intensity {1.0f};
            float bias {0.1f};
            float blur_sharpness {8.0f};
        } hbao_settings;
        /// Filament-style depth-aware half-res → full-res SSAO in deferred (NEAREST + 4-tap bilateral).
        bool ssao_high_quality_deferred_upsampling {true};
        /// Matches SSAO blur bilateral; used as 1/threshold for deferred upsample edge weighting.
        float ssao_deferred_upsample_bilateral_threshold {0.0625f};

        /**
         * Screen Space Reflections
         */
        bool screen_space_reflections {false};
        SSR::Settings ssr_settings;

        /**
         * FXAA Fast Approximate Antialiasing
         */
        bool fast_approximate_antialiasing {false};
        FxaaSettings fxaa;

        /**
         * Cascade shadow maps
         */
        bool cascade_shadow_maps = false;
        glm::uvec2 csm_resolution = {1024 * 4, 1024 * 4 };
        uint8_t csm_split_count = 3; // [2; 4]
        // CSM runtime options (no shader recompilation).
        bool csm_stable = true;
        bool csm_focus_shadow_casters = true;
        bool csm_depth_clamp = true;

        /**
         * Percentage-Closer Filtering for CSM
         */
        bool csm_pcf = true;

        /**
         * Shadow bias controls (runtime uniforms; no shader recompilation).
         *
         * Filament-style:
         * - normal bias is in world units (scaled from world-space texel size)
         * - receiver-plane bias reduces acne for filtered shadows
         */
        float csm_normal_bias_scale = 1.0f;
        bool csm_receiver_plane_bias = true;
        float csm_receiver_plane_bias_scale = 1.0f;
        float csm_constant_bias = 0.0f;
        /**
         * Shadow far attenuation: shadows fade out at distance to prevent harsh pop-in/pop-out.
         * Set to 0.0 to disable. Default is the camera far plane.
         */
        float csm_far_attenuation_distance = 0.0f;

        /**
         * Micro-shadowing for CSM
         */
        bool csm_micro_shadowing = true;

        /**
         *
         */
        bool bloom {false};
        float bloom_extract_threshold {1.0f};
        float bloom_strength {1.0f};
        /// Pyramid mip count for down/up passes (Filament-style bloom), clamped to ~12.
        uint32_t bloom_blur_iteration_count {8};
        /// Bloom plane height in pixels; width follows screen aspect (weak resolution dependence).
        uint32_t bloom_buffer_height {512};
        /// Reinhard-style weights on first downfilter tap (Filament fireflies path when no TAA).
        bool bloom_fireflies_reduction {true};
        /// Compress very bright peaks before threshold; 0 = disabled (Filament highlight = inf).
        float bloom_highlight {0.0f};

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

        /**
         * CPU tiled light culling (deferred)
         *
         * NOTE: tile size is compile-time in shaders via define.
         * Changing it requires shader recompilation.
         */
        uint32_t light_tile_size = 16; // pixels per tile edge

        /**
         * Debug overlay: visualize per-tile light list size in deferred shading.
         *
         * NOTE: compile-time via define; changing requires shader recompilation.
         */
        bool debug_light_tiles = false;

        /**
         * Use RenderQueue with material sorting for optimal state changes
         *
         * When enabled, instances are sorted by shader_index to minimize
         * expensive shader switches. Translucent objects maintain back-to-front order.
         */
        bool sorted_rendering = true;

        /**
         * Enable indirect draw rendering (Multi-Draw Indirect)
         *
         * When enabled:
         * - Geometry is batched into global pools by attribute type (same as geometry_pool_batching)
         * - Multiple objects rendered with single glMultiDrawElementsIndirect call
         * - Per-draw textures indexed via gl_DrawID (bindless)
         *
         * Requires: OpenGL 4.3+, GL_ARB_shader_draw_parameters, GL_ARB_bindless_texture
         */
        bool indirect_draw = false;

        /**
         * Pack indexed mesh geometry into global GeometryPool (shared VBO/EBO per vertex layout).
         *
         * When enabled (without indirect_draw): legacy renderer uses glDrawElements(BaseVertex)(Instanced)
         * with MeshDrawInfo offsets — fewer unique GPU buffers, same draw-call count as non-pooled.
         * When indirect_draw is true, pooling is implied; this flag can stay off.
         *
         * Set before loading meshes that call .batched(geometry_batching_enabled). Runtime toggles do not
         * rebuild already-created meshes.
         */
        bool geometry_pool_batching = false;

        /**
         * True when geometry should be placed in GeometryPool during mesh/stream build.
         */
        [[nodiscard]] static bool usesGeometryPool(const RendererSettings& s) noexcept {
            return s.indirect_draw || s.geometry_pool_batching;
        }

        /**
         * Use persistent mapped buffers for indirect draw SSBOs and command buffers
         *
         * When enabled, SSBOs are created with GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
         * so they stay permanently mapped.  mapData() becomes a simple memcpy instead
         * of glMapNamedBufferRange + glUnmapNamedBuffer, eliminating ~10ms/frame of
         * CPU overhead from 1000+ map/unmap cycles.
         *
         * Requires: GL_ARB_buffer_storage (OpenGL 4.4+)
         */
        bool persistent_buffer_mapping = false;

        /**
         * Triple-buffer indirect draw SSBOs and command buffers
         *
         * When enabled with indirect draw, instance_ssbo, material_ssbo, and the
         * indirect command buffer use TripleBuffer (3 backing buffers). Each fence()
         * advances the slot; waitFence() before writing waits for the slot about to
         * be overwritten. Reduces glClientWaitSync stalls when the GPU is still
         * using the previous frame's data.
         *
         * Best used with persistent_buffer_mapping.
         */
        bool triple_buffer_indirect = false;

        /**
         * Global flag to enable geometry batching during asset loading
         *
         * Set by the engine from usesGeometryPool(settings) (indirect_draw or geometry_pool_batching).
         * Asset loaders should pass this to VertexStream::Builder::batched(...).
         *
         * Note: This is a static flag because asset loading happens before
         * the renderer is fully constructed.
         */
        static inline bool geometry_batching_enabled = false;

        /**
         * Runtime mirror of effective global_model_instance_ssbo (after GPU + sorted_rendering checks).
         * Used by InstancedInstance and RenderQueue without plumbing Renderer& everywhere.
         */
        static inline bool global_model_instance_ssbo_active = false;

        /**
         * Pack ModelInstance, InstancedInstance, and terrain instanced draws into one SSBO (model_buffer)
         * indexed by gl_BaseInstance + gl_InstanceID. Draws use glDraw*Instanced*BaseInstance (incl. count=1).
         *
         * Requires: GL_ARB_base_instance (or OpenGL 4.2+), GL_ARB_shader_draw_parameters (or 4.3+) for gl_BaseInstance,
         * and sorted_rendering. Incompatible with indirect_draw (disabled automatically when indirect is on).
         * Shaders: ENGINE_GLOBAL_MODEL_INSTANCE_SSBO (see RenderSettingsShaderDefiner).
         */
        bool global_model_instance_ssbo = false;

        [[nodiscard]] bool ambient_occlusion_enabled() const noexcept {
            return ambient_occlusion_mode != AmbientOcclusionMode::None;
        }

        class Builder {
        private:
            /**
             *  Normal mapping
             */
            bool normal_mapping {true};

            AmbientOcclusionMode ambient_occlusion_mode_ {AmbientOcclusionMode::None};
            SSAO::Settings ssao_cfg;
            std::optional<SsaoFilamentQuality> ssao_filament_quality_;
            HbaoSettings hbao_cfg;

            /**
             * Screen Space Reflections
             */
            bool screen_space_reflections {false};
            SSR::Settings ssr_cfg;

            /**
             * FXAA Fast Approximate Antialiasing
             */
            bool fast_approximate_antialiasing {false};
            FxaaSettings fxaa_cfg;

            /**
             * Cascade shadow maps
             */
            bool cascade_shadow_maps = false;
            glm::uvec2 csm_resolution = {1024 * 2, 1024 * 2 };
            uint8_t csm_split_count = 3; // [2; 4]
            bool csm_stable_ = true;
            bool csm_focus_shadow_casters_ = true;
            bool csm_depth_clamp_ = true;

            /**
             * Percentage-Closer Filtering for CSM
             */
            bool csm_pcf = true;

            // Shadow bias controls (runtime uniforms)
            float csm_normal_bias_scale_ = 1.0f;
            bool csm_receiver_plane_bias_ = true;
            float csm_receiver_plane_bias_scale_ = 1.0f;
            float csm_constant_bias_ = 0.0f;

            /**
             * Micro-shadowing for CSM
             */
            bool csm_micro_shadowing = true;

            /**
             *
             */
            bool bloom {false};
            float bloom_ex_threshold {1.0f};
            float bloom_str {1.0f};
            uint32_t bloom_blur_it_count {8};
            uint32_t bloom_buffer_height_ {512};
            bool bloom_fireflies_reduction_ {true};
            float bloom_highlight_ {0.0f};

            // Post processing
            Tonemapper tonemapper {Tonemapper::Exponential};
            float exposure {1.0f};
            float gamma {2.2f};
            float white_balance_temperature {0.0f};
            float white_balance_tint {0.0f};
            float contrast {1.0f};
            float saturation {1.0f};
            glm::vec3 lift {0.0f};
            glm::vec3 grade_gamma {1.0f};
            glm::vec3 gain {1.0f};
            bool lut_enabled {false};
            std::string lut_texture {};
            float lut_intensity {1.0f};

            bool composite_white_balance_ {true};
            bool composite_color_grading_ {true};
            bool composite_display_gamma_ {true};

            // Height fog
            bool height_fog_include_in_composite_shader_ {false};
            bool height_fog_enabled_ {false};
            glm::vec3 height_fog_color_ {0.6f, 0.7f, 0.8f};
            float height_fog_density_ {0.02f};
            float height_fog_height_ {0.0f};
            float height_fog_falloff_ {0.15f};
            float height_fog_start_distance_ {0.0f};

            // Wind (compile-time)
            WindQuality wind_quality_ {WindQuality::Good};

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
            bool bounding_box = false;

            // CPU tiled light culling (compile-time in shaders)
            uint32_t light_tile_size_ = 16;
            bool debug_light_tiles_ = false;

            // Indirect draw (Multi-Draw Indirect)
            bool indirect_draw_ = false;
            bool geometry_pool_batching_ = false;
            bool persistent_buffer_mapping_ = false;
            bool triple_buffer_indirect_ = false;
            bool global_model_instance_ssbo_ = false;
        public:
            Builder& enable_normal_mapping();
            Builder& disable_normal_mapping();

            Builder& enable_ssao();
            Builder& disable_ssao();
            Builder& enable_hbao();
            Builder& ambient_occlusion_mode(AmbientOcclusionMode mode);
            Builder& ssao_settings(SSAO::Settings settings);
            /** Filament-quality SAO sample/spiral (PostProcessManager SSAO path). */
            Builder& ssao_filament_quality(SsaoFilamentQuality q);
            Builder& hbao_settings(HbaoSettings settings);
            Builder& ssao_sample_count(glm::vec2 count);
            Builder& ssao_spiral_turns(float turns);
            Builder& ssao_power(float power);
            Builder& ssao_bias(float bias);

            Builder& enable_ssr();
            Builder& disable_ssr();
            Builder& ssr_settings(SSR::Settings settings);

            Builder& enable_fxaa();
            Builder& disable_fxaa();
            Builder& fxaa_settings(FxaaSettings settings);
            Builder& fxaa_quality(FxaaQualityPreset preset);

            Builder& enable_csm();
            Builder& disable_csm();
            Builder& csm_texture_resolution(glm::uvec2 resolution);
            Builder& csm_count(uint32_t count);
            Builder& csm_stable(bool enabled);
            Builder& csm_focus_shadow_casters(bool enabled);
            Builder& csm_depth_clamp(bool enabled);
            Builder& csm_enable_pcf();
            Builder& csm_disable_pcf();
            Builder& csm_enable_micro_shadowing();
            Builder& csm_disable_micro_shadowing();

            // Shadow bias tuning (runtime uniforms)
            Builder& csm_normal_bias_scale(float scale);
            Builder& csm_receiver_plane_bias(bool enabled);
            Builder& csm_receiver_plane_bias_scale(float scale);
            Builder& csm_constant_bias(float bias_world_units);

            Builder& enable_bloom();
            Builder& disable_bloom();
            Builder& bloom_extract_threshold(float threshold);
            Builder& bloom_strength(float strength);
            Builder& bloom_blur_iteration_count(uint32_t count);
            Builder& bloom_buffer_height(uint32_t height_px);
            Builder& bloom_fireflies_reduction(bool enabled);
            /// 0 disables highlight compression; otherwise 1/x style crush (see Filament BloomOptions.highlight).
            Builder& bloom_highlight(float highlight);

            // Post processing
            Builder& postprocess_tonemapper(Tonemapper tonemapper);
            Builder& postprocess_exposure(float exposure);
            Builder& postprocess_gamma(float gamma);
            Builder& postprocess_white_balance(float temperature, float tint);
            Builder& postprocess_contrast(float contrast);
            Builder& postprocess_saturation(float saturation);
            Builder& postprocess_lift_gamma_gain(glm::vec3 lift, glm::vec3 gamma, glm::vec3 gain);
            Builder& postprocess_lut(const std::string& texture_name, float intensity = 1.0f);
            Builder& postprocess_disable_lut();

            /** Compile-time strips white balance in composite (requires shader recompilation). */
            Builder& composite_shader_white_balance(bool enabled);
            /** Compile-time strips lift/gamma/gain, contrast, saturation in composite. */
            Builder& composite_shader_color_grading(bool enabled);
            /** Compile-time strips final display gamma in composite. */
            Builder& composite_shader_display_gamma(bool enabled);

            // Height fog (exponential height fog)
            Builder& enable_height_fog();
            Builder& disable_height_fog();
            Builder& height_fog_color(glm::vec3 color);
            Builder& height_fog_density(float density);
            Builder& height_fog_height(float height);
            Builder& height_fog_falloff(float falloff);
            Builder& height_fog_start_distance(float distance);

            // Wind quality (compile-time; requires shader recompilation)
            Builder& wind_quality(WindQuality quality);

            Builder& enable_specular_aa();
            Builder& disable_specular_aa();
            Builder& specular_aa_threshold(float threshold);
            Builder& specular_aa_variance(float variance);

            Builder& debug_light_radius();
            Builder& debug_coordinate_system_axes();
            Builder& debug_bounding_box();
            Builder& light_tile_size(uint32_t tile_size_px);
            Builder& debug_light_tiles();

            // Indirect draw (Multi-Draw Indirect)
            Builder& enable_indirect_draw();
            Builder& disable_indirect_draw();

            // Shared geometry buffers (GeometryPool) without multi-draw indirect
            Builder& enable_geometry_pool_batching();
            Builder& disable_geometry_pool_batching();

            // Persistent mapped buffers for indirect draw
            Builder& enable_persistent_buffer_mapping();
            Builder& disable_persistent_buffer_mapping();

            // Triple-buffer indirect draw buffers
            Builder& enable_triple_buffer_indirect();
            Builder& disable_triple_buffer_indirect();

            /** See RendererSettings::global_model_instance_ssbo */
            Builder& enable_global_model_instance_ssbo();
            Builder& disable_global_model_instance_ssbo();

            RendererSettings build();
        };

        static Builder builder() { return {}; }

        /**
         * True if `other` would produce the same ENGINE_SETTINGS_COMPOSITE_* defines for composite.frag.
         */
        [[nodiscard]] bool sameCompositeShaderDefines(const RendererSettings& other) const noexcept;
    };
}
