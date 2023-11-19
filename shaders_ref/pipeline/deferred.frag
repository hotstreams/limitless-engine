ENGINE::COMMON

#include "../functions/reconstruct_position.glsl"
#include "../shading/shading_context.glsl"
#include "../lighting/lighting.glsl"

/** GBUFFER */

// UNSIGNED NORMALIZED [0; 1]
// RGBA8 - RGB - base color, A - ao

// SIGNED NORMALIZED [-1; 1]
// RGB - normal

// UNSIGNED NORMALIZED [0; 1]
// R - roughness, G - metallic, B - shading model (uint)

// FLOATING POINT
// RGB - emissive

/** */

uniform sampler2D base_texture;
uniform sampler2D normal_texture;
uniform sampler2D props_texture;
uniform sampler2D depth_texture;
uniform sampler2D emissive_texture;

in vec2 uv;

out vec3 color;

//#if defined (SCREEN_SPACE_AMBIENT_OCCLUSION)
//    uniform sampler2D ssao_texture;
//#endif

//TODO: refactor - separate responsibilities

//#ifdef SCREEN_SPACE_REFLECTIONS
//    uniform sampler2D ssr_texture;
//    uniform float _ssr_strength;
//#endif

void main() {
    vec3 P = reconstructPosition(uv, texture(depth_texture, uv).r);
    vec3 normal = texture(normal_texture, uv).rgb;
    vec4 base = texture(base_texture, uv).rgba;
    vec3 props = texture(props_texture, uv).rgb;
    uint shading_model = uint(props.b * 255.0);

//#if defined (SCREEN_SPACE_AMBIENT_OCCLUSION)
//    base.a *= texture(ssao_texture, uv).r;
//#endif

    ShadingContext sctx = computeShadingContext(
        base.rgb,
        props.g,
        P,
        props.r,
        normal,
        base.a
    );
    color = computeLights(sctx);

//    color = getFragmentColor(base.rgb, base.a, normal, P, metallic, roughness, shading_model);
//    color += texture(emissive_texture, uv).rgb;
//  just add for now for test
//#ifdef SCREEN_SPACE_REFLECTIONS
//    //  TODO: move to indirect lighting
//    color += texture(ssr_texture, uv).rgb *_ssr_strength;
//#endif
}
