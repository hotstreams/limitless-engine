#include <limitless/renderer/render_settings_shader_definer.hpp>
#include <limitless/renderer/renderer_settings.hpp>
#include <limitless/core/shader/shader.hpp>
#include <limitless/core/shader/shader_define_replacer.hpp>

using namespace Limitless;

std::string RenderSettingsShaderDefiner::getDefine(const RendererSettings &settings) {
    std::string s;

    if (settings.normal_mapping) {
        s.append("#define ENGINE_SETTINGS_NORMAL_MAPPING\n");
    }

    if (settings.fast_approximate_antialiasing) {
        s.append("#define ENGINE_SETTINGS_FXAA_QUALITY_PRESET " + std::to_string(static_cast<int>(settings.fxaa.quality)) + '\n');
    }

    if (settings.cascade_shadow_maps) {
        s.append("#define ENGINE_SETTINGS_CSM\n");

        s.append("#define ENGINE_SETTINGS_CSM_SPLIT_COUNT " + std::to_string(settings.csm_split_count) + '\n');

        if (settings.csm_pcf) {
            s.append("#define ENGINE_SETTINGS_PCF\n");
        }
    }

    if (settings.ambient_occlusion_enabled()) {
        s.append("#define ENGINE_SETTINGS_SSAO\n");
    }

    if (settings.screen_space_reflections) {
        s.append("#define ENGINE_SETTINGS_SSR\n");

        if (settings.ssr_settings.intersection_distance_attenuation) {
            s.append("#define ENGINE_SETTINGS_SSR_INTERSECTION_DISTANCE_ATTENUATION\n");
        }

        if (settings.ssr_settings.iteration_count_attenuation) {
            s.append("#define ENGINE_SETTINGS_SSR_ITERATION_COUNT_ATTENUATION\n");
        }

        if (settings.ssr_settings.borders_attenuation) {
            s.append("#define ENGINE_SETTINGS_SSR_BORDERS_ATTENUATION\n");
        }

        if (settings.ssr_settings.fresnel_attenuation) {
            s.append("#define ENGINE_SETTINGS_SSR_FRESNEL_ATTENUATION\n");
        }

        if (settings.ssr_settings.camera_facing_attenuation) {
            s.append("#define ENGINE_SETTINGS_SSR_CAMERA_FACING_ATTENUATION\n");
        }

        if (settings.ssr_settings.clip_to_frustrum) {
            s.append("#define ENGINE_SETTINGS_SSR_CLIP_TO_FRUSTRUM\n");
        }

        if (settings.ssr_settings.refiniment) {
            s.append("#define ENGINE_SETTINGS_SSR_REFINEMENT\n");
        }
    }

    if (settings.csm_micro_shadowing) {
        s.append("#define ENGINE_SETTINGS_MICRO_SHADOWING\n");
    }

    if (settings.specular_aa) {
        s.append("#define ENGINE_SETTINGS_SPECULAR_AA\n");
        s.append("#define ENGINE_SETTINGS_SPECULAR_AA_THRESHOLD " + std::to_string(settings.specular_aa_threshold) + '\n');
        s.append("#define ENGINE_SETTINGS_SPECULAR_AA_VARIANCE " + std::to_string(settings.specular_aa_variance) + '\n');
    }

    // Post-processing compile-time switches
    // NOTE: any change here requires shader recompilation (Assets::recompileAssets with new RendererSettings).
    switch (settings.postprocess.tonemapper) {
        case RendererSettings::Tonemapper::ACES:
            s.append("#define ENGINE_SETTINGS_TONEMAPPER_ACES\n");
            break;
        case RendererSettings::Tonemapper::Filmic:
            s.append("#define ENGINE_SETTINGS_TONEMAPPER_FILMIC\n");
            break;
        case RendererSettings::Tonemapper::Exponential:
        default:
            s.append("#define ENGINE_SETTINGS_TONEMAPPER_EXPONENTIAL\n");
            break;
    }

    // Composite pass (single variant per shader compile; see RendererSettings::sameCompositeShaderDefines)
    if (settings.bloom) {
        s.append("#define ENGINE_SETTINGS_COMPOSITE_BLOOM\n");
    }
    if (settings.height_fog.include_in_composite_shader) {
        s.append("#define ENGINE_SETTINGS_COMPOSITE_HEIGHT_FOG\n");
    }
    if (settings.postprocess.lut_enabled) {
        s.append("#define ENGINE_SETTINGS_COMPOSITE_LUT\n");
    }
    if (settings.postprocess.composite_white_balance) {
        s.append("#define ENGINE_SETTINGS_COMPOSITE_WHITE_BALANCE\n");
    }
    if (settings.postprocess.composite_color_grading) {
        s.append("#define ENGINE_SETTINGS_COMPOSITE_COLOR_GRADING\n");
    }
    if (settings.postprocess.composite_display_gamma) {
        s.append("#define ENGINE_SETTINGS_COMPOSITE_DISPLAY_GAMMA\n");
    }

    // Wind quality is compile-time (shader permutations)
    s.append("#define ENGINE_SETTINGS_WIND_QUALITY " + std::to_string(static_cast<uint32_t>(settings.wind_quality)) + '\n');

    // CPU tiled light culling settings (compile-time)
    s.append("#define ENGINE_SETTINGS_LIGHT_TILE_SIZE " + std::to_string(settings.light_tile_size) + '\n');
    if (settings.debug_light_tiles) {
        s.append("#define ENGINE_SETTINGS_DEBUG_LIGHT_TILES\n");
    }

    if (settings.global_model_instance_ssbo) {
        s.append("#define ENGINE_GLOBAL_MODEL_INSTANCE_SSBO\n");
    }

    return s;
}
