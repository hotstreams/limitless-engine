LimitlessEngine::GLSL_VERSION
LimitlessEngine::Extensions

in vec2 fs_uv;

out vec4 color;

uniform sampler2D image;
uniform sampler2D image_depth;

uniform bool bloom;
uniform sampler2D bloom_image;

uniform bool gamma_correction;
uniform float gamma;

uniform bool tone_mapping;
uniform float exposure;

uniform bool vignette;
uniform float vignette_radius;
uniform float vignette_softness;

uniform bool tone_shading;
uniform float number_of_colors;
uniform float line_texel_offset;
uniform float line_multiplier;
uniform float line_bias;

uniform bool fxaa;

#include "glsl/fxaa.glsl"

void main()
{
    vec3 scene_color = texture(image, fs_uv).rgb;
    float scene_depth = texture(image_depth, fs_uv).r;
    vec2 resolution = textureSize(image, 0);

    if (fxaa) {
        scene_color = fxaaProcess(image, fs_uv);
    }

    if (bloom) {
        scene_color += texture(bloom_image, fs_uv).rgb;
    }

    if (tone_mapping) {
        scene_color = vec3(1.0) - exp(-scene_color * exposure);
    }

    if (gamma_correction) {
        scene_color = pow(scene_color, vec3(1.0 / gamma));
    }

    if (tone_shading) {
        scene_color = scene_color * vec3(0.666) + floor(scene_color * number_of_colors) / number_of_colors;
        vec2 inv_resolution = 1.0 / resolution;

        float right = texture(image_depth, inv_resolution * vec2(1.0 * line_texel_offset, 0.0) + fs_uv).r;
        float left = texture(image_depth, inv_resolution * vec2(-1.0 * line_texel_offset, 0.0) + fs_uv).r;

        float top = texture(image_depth, inv_resolution * vec2(0.0, -1.0 * line_texel_offset) + fs_uv).r;
        float down = texture(image_depth, inv_resolution * vec2(0.0, 1.0 * line_texel_offset) + fs_uv).r;

        float lines = 4.0 * scene_depth - right - left - top - down;
        lines *= line_multiplier;
        lines = pow(lines, line_bias);
        lines = clamp(lines, 0.0, 1.0);

        scene_color = mix(scene_color, vec3(0.0), lines);
    }

    if (vignette) {
        float len = length(gl_FragCoord.xy / resolution - vec2(0.5));
        float vig = smoothstep(vignette_radius, vignette_softness, len);
        scene_color = mix(scene_color, scene_color * vig, 0.5);
    }

    color = vec4(scene_color, 1.0);
}