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

void process(
    vec3 base_ddx,
    vec3 base_ddy,
    const float weight,
    const ivec2 index,
    const uint control,
    const vec2 texture_weight,
    const ivec2 texture_id,
    const vec3 i_normal,
    float h,
    inout tile_data mat) {

    float control_scale = DECODE_SCALE(control);
    float c_angle = DECODE_ROTATION(control);
    float blend = DECODE_BLEND(control);

    vec3 i_vertex = getVertexPosition();

    base_ddx *= control_scale;
    base_ddy *= control_scale;
    i_vertex *= control_scale;
    h *= control_scale;

    vec2 i_pos = vec2(index) * terrain_vertex_spacing * control_scale;

    vec2 i_uv = i_vertex.xz;
    vec4 i_dd = vec4(base_ddx.xz, base_ddy.xz);

    vec2 c_cs_angle = vec2(cos(c_angle), sin(c_angle));
    i_uv = rotate_vec2(i_uv, c_cs_angle);
    i_pos = rotate_vec2(i_pos, c_cs_angle);

    float world_normal = 1.0;

    vec3 T = normalize(base_ddx);
    vec3 B = -normalize(base_ddy);

    #define FAST_WORLD_NORMAL(n) fma(T, vec3(n.x), fma(B, vec3(n.z), i_normal * vec3(n.y)))

    float sharpness = blend_sharpness;

    if (blend < 1.0) {
        int id = texture_id[0];
        float id_w = texture_weight[0];
        float id_scale = terrain_texture_uv_scale_array[id];
        vec4 id_dd = i_dd * id_scale;

        vec2 id_pos = i_pos;
        vec2 uv_center = floor(fma(id_pos, vec2(id_scale), vec2(0.5)));
        vec2 id_detile = fma(random(uv_center), 2.0, -1.0) * terrain_texture_detile_array[id] * 6.28318;
        vec2 id_cs_angle = vec2(cos(id_detile.x), sin(id_detile.x));
        
        vec2 id_uv = rotate_vec2(fma(i_uv, vec2(id_scale), -uv_center), id_cs_angle) + uv_center + id_detile.y - 0.5;

        id_cs_angle = vec2(
            fma(id_cs_angle.x, c_cs_angle.x, -id_cs_angle.y * c_cs_angle.y),
            fma(id_cs_angle.y, c_cs_angle.x, id_cs_angle.x * c_cs_angle.y));
        
        id_dd.xy = rotate_vec2(id_dd.xy, id_cs_angle);
        id_dd.zw = rotate_vec2(id_dd.zw, id_cs_angle);

        vec4 alb = textureGrad(terrain_albedo_texture, vec3(id_uv, float(id)), id_dd.xy, id_dd.zw);
        vec4 nrm = textureGrad(terrain_normal_texture, vec3(id_uv, float(id)), id_dd.xy, id_dd.zw);
        
        nrm.xyz = fma(nrm.xzy, vec3(2.0), vec3(-1.0));
        nrm.xz = rotate_vec2(nrm.xz, id_cs_angle);

        world_normal = FAST_WORLD_NORMAL(nrm).y;

        float id_weight = exp2(sharpness * log2(weight + id_w + alb.a)) * weight;
        mat.albedo_height = fma(alb, vec4(id_weight), mat.albedo_height);
        mat.normal_roughness = fma(nrm, vec4(id_weight), mat.normal_roughness);
        mat.normal_depth = fma(terrain_texture_normal_depth_array[id], id_weight, mat.normal_depth);
        mat.total_weight += id_weight;
    }

    if (blend > 0.0 && texture_id[1] != texture_id[0]) {
        int id = texture_id[1];
        float id_w = texture_weight[1];
        float id_scale = terrain_texture_uv_scale_array[id];
        vec4 id_dd = i_dd * id_scale;

        vec2 id_pos = i_pos;
        vec2 uv_center = floor(fma(id_pos, vec2(id_scale), vec2(0.5)));
        vec2 id_detile = fma(random(uv_center), 2.0, -1.0) * terrain_texture_detile_array[id] * 6.28318;
        vec2 id_cs_angle = vec2(cos(id_detile.x), sin(id_detile.x));
        
        vec2 id_uv = rotate_vec2(fma(i_uv, vec2(id_scale), -uv_center), id_cs_angle) + uv_center + id_detile.y - 0.5;
        
        id_cs_angle = vec2(
            fma(id_cs_angle.x, c_cs_angle.x, -id_cs_angle.y * c_cs_angle.y),
            fma(id_cs_angle.y, c_cs_angle.x, id_cs_angle.x * c_cs_angle.y));
        
        id_dd.xy = rotate_vec2(id_dd.xy, id_cs_angle);
        id_dd.zw = rotate_vec2(id_dd.zw, id_cs_angle);

        vec4 alb = textureGrad(terrain_albedo_texture, vec3(id_uv, float(id)), id_dd.xy, id_dd.zw);
        vec4 nrm = textureGrad(terrain_normal_texture, vec3(id_uv, float(id)), id_dd.xy, id_dd.zw);
        
        nrm.xyz = fma(nrm.xzy, vec3(2.0), vec3(-1.0));
        nrm.xz = rotate_vec2(nrm.xz, id_cs_angle);

        float id_weight = exp2(sharpness * log2(weight + id_w + alb.a * clamp(world_normal, 0.0, 1.0))) * weight;
        mat.albedo_height = fma(alb, vec4(id_weight), mat.albedo_height);
        mat.normal_roughness = fma(nrm, vec4(id_weight), mat.normal_roughness);
        mat.normal_depth = fma(terrain_texture_normal_depth_array[id], id_weight, mat.normal_depth);
        mat.total_weight += id_weight;
    }
}