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
    // Convert world-space position to texel-space by dividing by vertex_spacing
    // This ensures offsets of +1, +2 etc represent actual texel neighbors
    // NOTE: NO mod here! Mod happens at texel fetch time, not during coordinate conversion
    return vertex_position * terrain_vertex_density;
}

vec2 getTerrainUV(vec2 texel_uv) {
    // Convert texel-space to normalized UV [0, 1]
    // Using texture() with bilinear filtering (not texelFetch like Terrain3D)
    return mod(texel_uv, terrain_size) * terrain_texel_size;
}

vec2 getChunkUV(vec2 uv) {
    return uv * (terrain_size * 1.0 / terrain_vertex_spacing);
}
