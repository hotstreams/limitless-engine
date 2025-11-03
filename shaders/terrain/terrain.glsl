#include "../functions/is_on_tile_border.glsl"
#include "./terrain_control.glsl"
#include "../functions/util.glsl"

struct tile_data {
    vec4 albedo_height;
    vec4 normal_roughness;
    float normal_depth;
    float total_weight;
};

uint fetchTerrainControl(ivec2 texel) {
    return texelFetch(terrain_control_texture, texel, 0).r;
}

float fetchTerrainHeight(ivec2 texel) {
    return texelFetch(terrain_height_texture, texel, 0).r * terrain_height_scale;
}

vec4 fetchColor(ivec2 texel) {
    return texelFetch(terrain_color_map, texel, 0);
}

float getTerrainHeight(vec2 uv) {
    return texture(terrain_height_texture, uv).r * terrain_height_scale;
}

vec2 getTerrainTexelUV(vec2 vertex_position) {
    return mod(vertex_position, terrain_size * terrain_vertex_spacing);
}

vec2 getTerrainUV(vec2 texel_uv) {
    return texel_uv / (terrain_size * terrain_vertex_spacing);
}

vec2 getChunkUV(vec2 uv) {
    return uv * (terrain_size * 1.0 / terrain_vertex_spacing);
}
