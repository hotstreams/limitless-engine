Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings

#include "../scene.glsl"
#include "../shading/shading.glsl"
#include "../../functions/reconstruct_position.glsl"
#include "../../functions/gamma_correction.glsl"

#include "./gbuffer_input.glsl"

in vec2 uv;

out vec3 color;

void main() {
    // sample parameters from textures
    vec3 P = reconstructPosition(uv, texture(depth_texture, uv).r);
    vec3 normal = texture(normal_texture, uv).rgb;
    vec4 base = texture(base_texture, uv).rgba;
    vec3 props = texture(props_texture, uv).rgb;
    float roughness = props.r;
    float metallic = props.g;
    uint shading_model = uint(props.b * 255.0);

    color = getFragmentColor(base.rgb, base.a, normal, P, metallic, roughness, shading_model);
    color += texture(emissive_texture, uv).rgb;
}
