#include <limitless/renderer/render_settings_shader_definer.hpp>
#include <limitless/renderer/render_settings.hpp>
#include <limitless/core/shader/shader.hpp>
#include <limitless/core/shader/shader_defines.hpp>

using namespace Limitless;

RenderSettingsShaderDefiner::RenderSettingsShaderDefiner(const RenderSettings& settings) noexcept
    : settings {settings} {

}

void RenderSettingsShaderDefiner::define(Shader &shader) {
    std::string s;

    if (settings.normal_mapping) {
        s.append("#define ENGINE_SETTINGS_NORMAL_MAPPING\n");
    }

    if (settings.cascade_shadow_maps) {
        s.append("#define ENGINE_SETTINGS_CSM\n");

        s.append("#define ENGINE_SETTINGS_CSM_SPLIT_COUNT " + std::to_string(settings.csm_split_count) + '\n');

        if (settings.csm_pcf) {
            s.append("#define ENGINE_SETTINGS_PFC\n");
        }
    }

    if (settings.screen_space_ambient_occlusion) {
        s.append("#define ENGINE_SETTINGS_SSAO\n");
    }

//    if (settings.screen_space_reflections) {
//        s.append("#define SCREEN_SPACE_REFLECTIONS\n");
//
//        if (settings.screen_space_reflections_settings.intersection_distance_attenuation) {
//            s.append("#define SCREEN_SPACE_REFLECTIONS_INTERSECTION_DISTANCE_ATTENUATION\n");
//        }
//
//        if (settings.screen_space_reflections_settings.iteration_count_attenuation) {
//            s.append("#define SCREEN_SPACE_REFLECTIONS_ITERATION_COUNT_ATTENUATION\n");
//        }
//
//        if (settings.screen_space_reflections_settings.borders_attenuation) {
//            s.append("#define SCREEN_SPACE_REFLECTIONS_BORDERS_ATTENUATION\n");
//        }
//
//        if (settings.screen_space_reflections_settings.fresnel_attenuation) {
//            s.append("#define SCREEN_SPACE_REFLECTIONS_FRESNEL_ATTENUATION\n");
//        }
//
//        if (settings.screen_space_reflections_settings.camera_facing_attenuation) {
//            s.append("#define SCREEN_SPACE_REFLECTIONS_CAMERA_FACING_ATTENUATION\n");
//        }
//
//        if (settings.screen_space_reflections_settings.clip_to_frustrum) {
//            s.append("#define SCREEN_SPACE_REFLECTIONS_CLIP_TO_FRUSTRUM\n");
//        }
//
//        if (settings.screen_space_reflections_settings.refiniment) {
//            s.append("#define SCREEN_SPACE_REFLECTIONS_REFINEMENT\n");
//        }
//    }

    if (settings.csm_micro_shadowing) {
        s.append("#define ENGINE_SETTINGS_MICRO_SHADOWING\n");
    }

    shader.replaceKey(ENGINE_SHADER_DEFINE_NAMES[EngineShaderDefine::Settings], s);
}
