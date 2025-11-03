#include "../terrain/terrain.glsl"
#include "../terrain/terrain_debug.glsl"

void calculateTerrain(inout MaterialContext mctx) {
    vec3 vertex_position = getVertexWorldPosition().xyz;
    vec2 terrain_texel_uv = getTerrainTexelUV(vertex_position.xz);
    vec2 terrain_uv = getTerrainUV(terrain_texel_uv);
    vec2 chunk_uv = getChunkUV(terrain_uv);
    vec2 terrain_texel_base = floor(terrain_texel_uv);

    if (vertex_position.x < 0.0 || vertex_position.x >= terrain_size ||
         vertex_position.z < 0.0 || vertex_position.z >= terrain_size) {
         discard;
    }

    vec2 index_id = floor(terrain_texel_uv);
    vec2 weight = fract(terrain_texel_uv);
    vec2 invert = 1.0 - weight;
    vec4 weights = vec4(
        invert.x * weight.y,
        weight.x * weight.y,
        weight.x * invert.y,
        invert.x * invert.y
    );

    ivec2 index[4];
    const vec3 offsets = vec3(0, 1, 2);

    index[0] = ivec2(getTerrainTexelUV(terrain_texel_base + offsets.xy));
    index[1] = ivec2(getTerrainTexelUV(terrain_texel_base + offsets.yy));
    index[2] = ivec2(getTerrainTexelUV(terrain_texel_base + offsets.yx));
    index[3] = ivec2(getTerrainTexelUV(terrain_texel_base + offsets.xx));

    vec3 base_ddx = dFdxCoarse(vertex_position);
    vec3 base_ddy = dFdxCoarse(vertex_position);

    float region_mip = max(length(base_ddx.xz), length(base_ddy.xz));
    bool normal_bilerp = log2(region_mip * terrain_vertex_density * normal_bilerp_multiplier) < 0.0;
    bool tile_bilerp = log2(region_mip * terrain_vertex_density * tile_bilerp_multiplier) < 0.0;

    vec3 index_normal[4];
    float h[4];

    h[3] = fetchTerrainHeight(index[3]);
    h[2] = fetchTerrainHeight(index[2]);
    h[0] = fetchTerrainHeight(index[0]);
    index_normal[3] = normalize(vec3(h[3] - h[2], terrain_vertex_spacing, h[3] - h[0]));

    vec3 w_normal = index_normal[3];

    float vertex_camera_distance = length(getVertexPosition().xz - getCameraPosition().xz);

    float bias = mix(mipmap_bias,
        depth_blur + 1.0,
        smoothstep(0.0, 1.0, (vertex_camera_distance - bias_distance) * terrain_texel_size));

    base_ddx *= bias;
    base_ddy *= bias;

    if (normal_bilerp) {
        h[1] = fetchTerrainHeight(index[1]);
        float h4 = fetchTerrainHeight(ivec2(terrain_texel_base + offsets.yz));
        float h5 = fetchTerrainHeight(ivec2(terrain_texel_base + offsets.zy));
        float h6 = fetchTerrainHeight(ivec2(terrain_texel_base + offsets.zx));
        float h7 = fetchTerrainHeight(ivec2(terrain_texel_base + offsets.xz));

        index_normal[0] = normalize(vec3(h[0] - h[1], terrain_vertex_spacing, h[0] - h7));
        index_normal[1] = normalize(vec3(h[1] - h5, terrain_vertex_spacing, h[1] - h4));
        index_normal[2] = normalize(vec3(h[2] - h6, terrain_vertex_spacing, h[2] - h[1]));

        w_normal =
            index_normal[0] * weights[0] +
            index_normal[1] * weights[1] +
            index_normal[2] * weights[2] +
            index_normal[3] * weights[3];

        w_normal = normalize(w_normal);
    }

    vec4 color_map = fetchColor(index[3]);
    uvec4 control = uvec4(fetchTerrainControl(index[3]));

    if (tile_bilerp && bool(enable_tile_bilerp)) {
        vec4 col_map[4];
        col_map[0] = fetchColor(index[0]);
        col_map[1] = fetchColor(index[1]);
        col_map[2] = fetchColor(index[2]);
        col_map[3] = fetchColor(index[3]);
        color_map = col_map[0] * weights[0] + col_map[1] * weights[1] + col_map[2] * weights[2] + col_map[3] * weights[3];

        control = uvec4(
            fetchTerrainControl(index[0]),
            fetchTerrainControl(index[1]),
            fetchTerrainControl(index[2]),
            control[3]
        );
    }

    ivec4 t_id[2] = {
        ivec4(control >> uvec4(0u) & uvec4(0x3Fu)),
        ivec4(control >> uvec4(6u) & uvec4(0x3Fu))
    };

    ivec2 texture_ids[4] = ivec2[4](
        ivec2(t_id[0].x, t_id[1].x),
        ivec2(t_id[0].y, t_id[1].y),
        ivec2(t_id[0].z, t_id[1].z),
        ivec2(t_id[0].w, t_id[1].w)
    );

    vec4 weights_id_1 = vec4(control >> uvec4(12u) & uvec4(0xFFu)) * DIV_255;
    vec4 weights_id_0 = 1.0 - weights_id_1;

    vec2 t_weights[4] = vec2[4](
        vec2(weights_id_0[0], weights_id_1[0]),
        vec2(weights_id_0[1], weights_id_1[1]),
        vec2(weights_id_0[2], weights_id_1[2]),
        vec2(weights_id_0[3], weights_id_1[3]));

    if (tile_bilerp && bool(enable_tile_bilerp)) {
        t_weights = vec2[4](vec2(0), vec2(0), vec2(0), vec2(0));
        weights_id_0 *= weights;
        weights_id_1 *= weights;
        for (int i = 0; i < 4; i++) {
            vec2 w_0 = vec2(weights_id_0[i]);
            vec2 w_1 = vec2(weights_id_1[i]);
            ivec2 id_0 = texture_ids[i].xx;
            ivec2 id_1 = texture_ids[i].yy;
            t_weights[0] += fma(w_0, vec2(equal(texture_ids[0], id_0)), w_1 * vec2(equal(texture_ids[0], id_1)));
            t_weights[1] += fma(w_0, vec2(equal(texture_ids[1], id_0)), w_1 * vec2(equal(texture_ids[1], id_1)));
            t_weights[2] += fma(w_0, vec2(equal(texture_ids[2], id_0)), w_1 * vec2(equal(texture_ids[2], id_1)));
            t_weights[3] += fma(w_0, vec2(equal(texture_ids[3], id_0)), w_1 * vec2(equal(texture_ids[3], id_1)));
        }
    }

    tile_data mat = tile_data(vec4(0.0), vec4(0.0), 0.0, 0.0);

    float base_weight = (tile_bilerp && bool(enable_tile_bilerp)) ? weights[3] : 1.0;
    
    process(
        base_ddx,
        base_ddy,
        base_weight,
        index[3],
        control[3],
        t_weights[3],
        texture_ids[3],
        index_normal[3],
        h[3],
        mat);

    if (tile_bilerp && bool(enable_tile_bilerp)) {
        process(
            base_ddx,
            base_ddy,
            weights[2],
            index[2],
            control[2],
            t_weights[2],
            texture_ids[2],
            index_normal[2],
            h[2],
            mat);

        process(
            base_ddx,
            base_ddy,
            weights[1],
            index[1],
            control[1],
            t_weights[1],
            texture_ids[1],
            index_normal[1],
            h[1],
            mat);

        process(
            base_ddx,
            base_ddy,
            weights[0],
            index[0],
            control[0],
            t_weights[0],
            texture_ids[0],
            index_normal[0],
            h[0],
            mat);
    }

    float weight_inv = 1.0 / max(mat.total_weight, 1e-3);
    mat.albedo_height *= weight_inv;
    mat.normal_roughness *= weight_inv;
    mat.normal_depth *= weight_inv;

    mctx.color.xyz = mat.albedo_height.rgb * color_map.rgb;

    mctx.roughness = clamp(fma(color_map.a - 0.5, 2.0, mat.normal_roughness.a), 0.0, 1.0);
    mctx.metallic = 0.0;

    float ao = (1.0 - (mat.albedo_height.a * log(1.1))) * (1.0 - mat.normal_roughness.y);
    mctx.ao = clamp(1.0 - ao, mat.albedo_height.a, 1.0);

    mctx.vertex_normal = w_normal;

    #if (defined(ENGINE_MATERIAL_NORMAL_TEXTURE) || defined(ENGINE_MATERIAL_NORMAL_MAP)) && defined(ENGINE_SETTINGS_NORMAL_MAPPING)
        vec3 norm = normalize(mat.normal_roughness.xzy);

        norm.xy *= mat.normal_depth;
        norm.z = sqrt(max(0.0, 1.0 - dot(norm.xy, norm.xy)));

        mctx.normal = fma(normalize(norm), vec3(0.5), vec3(0.5));

        mctx.tangent = normalize(cross(w_normal, vec3(0.0, 0.0, 1.0)));
    #endif

    // Apply debug visualization if enabled
    //applyDebugVisualization(17, terrain_uv, control[3], mctx, mat, bilerp, texture_ids[3]);

/*
    if (bool(terrain_show_tiles)) {
        if (is_on_tile_border(vertex_position, vec3(terrain_vertex_spacing), 0.01)) {
            mctx.color.xyz = vec3(1.0, 0.0, 0.0);
        }
    }

    if (bool(terrain_show_terrain_size)) {
        if (is_on_tile_border(vertex_position, vec3(terrain_size), 0.01)) {
            mctx.color.xyz = vec3(0.0, 1.0, 0.0);
        }
    }
*/
}
