#include "../functions/is_on_tile_border.glsl"
#include "../functions/noise.glsl"
#include "./terrain_control.glsl"

uint getTerrainControl(vec2 uv) {
    return texelFetch(terrain_control_texture, ivec2(uv), 0).r;
}

vec3 getTerrainAlbedo(vec3 uv) {
    return texture(terrain_albedo_texture, uv).rgb;
}

float blend_weights(float weight, float detail) {
    weight = smoothstep(0.0, 1.0, weight);
    weight = sqrt(weight * 0.5);
    return clamp(weight, 0.0, 1.0);
}

vec2 rotate(vec2 v, float cosa, float sina) {
    return vec2(cosa * v.x - sina * v.y, sina * v.x + cosa * v.y);
}

uniform float blend_sharpness = 0.87;

vec3 height_blend(vec3 a_value, float a_height, vec3 b_value, float b_height, float blend) {
    float ma = max(a_height + (1.0 - blend), b_height + blend) - (1.001 - blend_sharpness);
    float b1 = max(a_height + (1.0 - blend) - ma, 0.0);
    float b2 = max(b_height + blend - ma, 0.0);
    return (a_value * b1 + b_value * b2) / (b1 + b2);
}

struct terrain_data {
    vec3 albedo;
    vec3 normal;
    vec3 orm;
};

float get_height(vec2 uv) {
    float height = texture(terrain_height_texture, uv).r * terrain_height_scale;
    height += value_noise(uv);
    return height;
}

vec3 getTerrainNormal(vec2 uv) {
    float terrain_texel_size = 1.0 / terrain_size;

    float hL = get_height(uv - vec2(terrain_texel_size, 0));
    float hR = get_height(uv + vec2(terrain_texel_size, 0));
    float hD = get_height(uv - vec2(0, terrain_texel_size));
    float hU = get_height(uv + vec2(0, terrain_texel_size));

    float u = hL - hR;
    float v = hD - hU;

    return normalize(vec3(u, 2.0 * terrain_vertex_spacing, v));
}

mat3 getTerrainTBN(vec3 normal) {
    mat3 T;
    T[0] = abs(normal.z) > 0.99 ? normalize(cross(normal, vec3(0, 1, 0))) : normalize(cross(normal, vec3(0, 0, 1)));
    T[1] = normalize(cross(normal, T[0]));
    T[2] = normal;
    return T;
}

vec2 getTerrainTexelUV(vec2 vertex_position) {
    return mod(vertex_position, terrain_size * terrain_vertex_spacing);
}

vec2 getTerrainUV(vec2 texel_uv) {
    return texel_uv / (terrain_size * terrain_vertex_spacing);
}

// returns UV [0, 1] in chunk space
vec2 getChunkUV(vec2 uv) {
    return uv * (terrain_size * 1.0 / terrain_vertex_spacing) / terrain_texture_scale;
}