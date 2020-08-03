GraphicsEngine::GLSL_VERSION
GraphicsEngine::Extensions

in vec2 fs_uv;

out vec4 color;

uniform sampler2D image;

uniform bool bloom;
uniform sampler2D bloom_image;

uniform bool gamma_correction;
uniform float gamma;

uniform bool tone_mapping;
uniform float exposure;

uniform bool vignette;
uniform float vignette_radius;
uniform float vignette_softness;

void main()
{
    vec3 scene_color = texture(image, fs_uv).rgb;

    if (bloom) {
        scene_color += texture(bloom_image, fs_uv).rgb;
    }

    if (tone_mapping) {
        scene_color = vec3(1.0) - exp(-scene_color * exposure);
    }

    if (gamma_correction) {
        scene_color = pow(scene_color, vec3(1.0 / gamma));
    }

    if (vignette) {
        float len = length(gl_FragCoord.xy / textureSize(image, 0) - vec2(0.5));
        float vig = smoothstep(vignette_radius, vignette_softness, len);
        scene_color.rgb = mix(scene_color.rgb, scene_color.rgb * vig, 0.5);
    }

    color = vec4(scene_color, 1.0);
}