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

    color = vec4(scene_color, 1.0);
}