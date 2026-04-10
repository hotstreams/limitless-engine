ENGINE::COMMON

#include "../functions/tone_mapping.glsl"
#include "../functions/color_grading.glsl"
#if defined(ENGINE_SETTINGS_COMPOSITE_HEIGHT_FOG)
#include "../functions/reconstruct_position.glsl"
#endif

in vec2 uv;

out vec3 color;

uniform sampler2D lightened;

#if defined(ENGINE_SETTINGS_COMPOSITE_HEIGHT_FOG)
uniform sampler2D depth_texture;
#endif

#if defined(ENGINE_SETTINGS_COMPOSITE_BLOOM)
uniform sampler2D bloom;
uniform float bloom_strength;
#endif

uniform sampler2D outline;

// Linear exposure before white balance / grading (Filament ColorGrading order subset).
uniform float tone_mapping_exposure;

#if defined(ENGINE_SETTINGS_COMPOSITE_DISPLAY_GAMMA)
uniform float gamma;
#endif

#if defined(ENGINE_SETTINGS_COMPOSITE_WHITE_BALANCE)
uniform float white_balance_temperature;
uniform float white_balance_tint;
#endif

#if defined(ENGINE_SETTINGS_COMPOSITE_COLOR_GRADING)
uniform float color_grading_contrast;
uniform float color_grading_saturation;
uniform vec3 color_grading_lift;
uniform vec3 color_grading_gamma;
uniform vec3 color_grading_gain;
#endif

#if defined(ENGINE_SETTINGS_COMPOSITE_LUT)
uniform float color_grading_lut_intensity;
uniform float color_grading_lut_size;
uniform sampler3D color_grading_lut;
#endif

#if defined(ENGINE_SETTINGS_COMPOSITE_HEIGHT_FOG)
uniform int height_fog_enabled;
uniform vec3 height_fog_color;
uniform float height_fog_density;
uniform float height_fog_height;
uniform float height_fog_falloff;
uniform float height_fog_start_distance;
#endif

void main() {
    color = texture(lightened, uv).rgb;

#if defined(ENGINE_SETTINGS_COMPOSITE_BLOOM)
    vec3 bloom_color = texture(bloom, uv).rgb * bloom_strength;
    color += bloom_color;
#endif

#if defined(ENGINE_SETTINGS_COMPOSITE_HEIGHT_FOG)
    if (height_fog_enabled != 0) {
        float depth = texture(depth_texture, uv).r;
        if (depth < 0.999999f) {
            vec3 ws = reconstructPosition(uv, depth);

            float dist = length(ws - getCameraPosition());
            float d = max(dist - height_fog_start_distance, 0.0);

            float height_delta = clamp(ws.y - height_fog_height, -100.0, 100.0);
            float density = height_fog_density * exp(-height_fog_falloff * height_delta);

            float fog_amount = 1.0 - exp(-density * d);
            fog_amount = clamp(fog_amount, 0.0, 1.0);
            color = mix(color, height_fog_color, fog_amount);
        }
    }
#endif

    color *= tone_mapping_exposure;

#if defined(ENGINE_SETTINGS_COMPOSITE_WHITE_BALANCE)
    color = applyWhiteBalance(color, white_balance_temperature, white_balance_tint);
#endif

#if defined(ENGINE_SETTINGS_COMPOSITE_COLOR_GRADING)
    color = applyLiftGammaGain(color, color_grading_lift, color_grading_gamma, color_grading_gain);
    color = applyContrast(color, color_grading_contrast);
    color = applySaturation(color, color_grading_saturation);
#endif

    color = toneMappingCurve(color);

    color = clamp(color, 0.0, 1.0);

#if defined(ENGINE_SETTINGS_COMPOSITE_LUT)
    {
        float size = max(color_grading_lut_size, 2.0);
        vec3 coord = color * ((size - 1.0) / size) + (0.5 / size);
        vec3 lut = texture(color_grading_lut, coord).rgb;
        color = mix(color, lut, clamp(color_grading_lut_intensity, 0.0, 1.0));
    }
#endif

#if defined(ENGINE_SETTINGS_COMPOSITE_DISPLAY_GAMMA)
    color = pow(color, vec3(1.0 / gamma));
#endif

    //color += texture(outline, uv).rgb;
}
