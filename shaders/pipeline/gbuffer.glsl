#include "../functions/reconstruct_position.glsl"

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

uniform sampler2D _base_texture;
uniform sampler2D _normal_texture;
uniform sampler2D _props_texture;
uniform sampler2D _depth_texture;
uniform sampler2D _emissive_texture;

vec4 getGBufferColor(vec2 uv) {
    return texture(_base_texture, uv);
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
