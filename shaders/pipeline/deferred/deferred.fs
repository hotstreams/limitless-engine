Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings

#define DIRECTIONAL_CSM
#define DIRECTIONAL_SPLIT_COUNT 3

#include "../scene.glsl"
#include "../lighting/lighting.glsl"
#include "../../functions/reconstruct_position.glsl"
#include "../../functions/gamma_correction.glsl"

in vec2 uv;

uniform sampler2D albedo_texture;
uniform sampler2D normal_texture;
uniform sampler2D props_texture;
uniform sampler2D depth_texture;

out vec3 frag_color;

void main() {
    vec3 P = reconstructPosition(uv, texture(depth_texture, uv).r);
    vec4 N = texture(normal_texture, uv).rgba;
    vec4 albedo = texture(albedo_texture, uv).rgba;
    vec2 props = texture(props_texture, uv).rg;

    float metallic = props.r;
    float roughness = props.g;

    uint shading_model = uint(N.a);

    frag_color = getFragmentColor(albedo.rgb, albedo.a, N.xyz, P, uv, metallic, roughness, shading_model);

    frag_color = gammaCorrection(frag_color, 2.2);
}
