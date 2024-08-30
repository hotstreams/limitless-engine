#include "../functions/reconstruct_position.glsl"

/** GBUFFER */

// UNSIGNED NORMALIZED [0; 1]
// RGB - base color

// SIGNED NORMALIZED [-1; 1]
// RGB - world space normal

// UNSIGNED NORMALIZED [0; 1]
// R - roughness, G - metallic, B - ao

// FLOATING POINT
// RGB - emissive

// UNSIGNED NORMALIZED
// R - shading model (uint), G - object type

// UNSIGNED NORMALIZED [0; 1]
// RGB - outline color, A - outline mask

/** */

uniform sampler2D _base_texture;
uniform sampler2D _normal_texture;
uniform sampler2D _props_texture;
uniform sampler2D _info_texture;
uniform sampler2D _emissive_texture;
uniform sampler2D _outline_texture;
uniform sampler2D _depth_texture;

vec3 getGBufferColor(vec2 uv) {
    return texture(_base_texture, uv).rgb;
}

vec3 getGBufferNormal(vec2 uv) {
    return texture(_normal_texture, uv).xyz;
}

vec3 getGBufferProps(vec2 uv) {
    return texture(_props_texture, uv).rgb;
}

vec3 getGBufferEmissiveColor(vec2 uv) {
    return texture(_emissive_texture, uv).rgb;
}

float getGBufferDepth(vec2 uv) {
    return texture(_depth_texture, uv).r;
}

vec3 getGBufferPosition(vec2 uv) {
    return reconstructPosition(uv, getGBufferDepth(uv));
}

vec2 getGBufferInfo(vec2 uv) {
    return texture(_info_texture, uv).rg;
}

vec4 getGBufferOutline(vec2 uv) {
    return texture(_outline_texture, uv).rgba;
}