#include "./terrain.glsl"
#include "../functions/stochastic.glsl"

terrain_data getTerrainData(vec2 uv, const terrain_control tctrl) {
    terrain_data data;

    vec2 uv1, uv2, uv3;
    vec3 weights;
    vec4 duvd;
    _get_stochastic_data(uv, uv1, uv2, uv3, duvd, weights);

    data.albedo = get_detiled_texture(uv1, uv2, uv3, duvd, weights, terrain_albedo_texture, tctrl.base_id);
    data.normal = get_detiled_texture(uv1, uv2, uv3, duvd, weights, terrain_normal_texture, tctrl.base_id);
    data.orm = get_detiled_texture(uv1, uv2, uv3, duvd, weights, terrain_orm_texture, tctrl.base_id);

    if (tctrl.blend > 0.0) {
        vec3 extra_albedo = get_detiled_texture(uv1, uv2, uv3, duvd, weights, terrain_albedo_texture, tctrl.extra_id);
        vec3 extra_normal = get_detiled_texture(uv1, uv2, uv3, duvd, weights, terrain_normal_texture, tctrl.extra_id);
        vec3 extra_orm = get_detiled_texture(uv1, uv2, uv3, duvd, weights, terrain_orm_texture, tctrl.extra_id);

        //TODO: height blending unpack?
        data.albedo = height_blend(data.albedo, 1.0, extra_albedo, 1.0, tctrl.blend);
        data.normal = height_blend(data.normal, 1.0, extra_normal, 1.0, tctrl.blend);
        data.orm = height_blend(data.orm, 1.0, extra_orm, 1.0, tctrl.blend);
    }

    return data;
}

void calculateTerrain(inout MaterialContext mctx) {
    vec3 vertex_position = getVertexPosition();
    vec2 terrain_texel_uv = getTerrainTexelUV(vertex_position.xz);
    vec2 terrain_uv = getTerrainUV(terrain_texel_uv);
    vec2 chunk_uv = getChunkUV(terrain_uv);

    vec2 terrain_texel_base = floor(terrain_texel_uv);

    vec2 terrain_adjacent_texel_uv[4] = {
        getTerrainTexelUV(terrain_texel_base),
        getTerrainTexelUV(terrain_texel_base + vec2(1.0, 0.0)),
        getTerrainTexelUV(terrain_texel_base + vec2(0.0, 1.0)),
        getTerrainTexelUV(terrain_texel_base + vec2(1.0, 1.0))
    };

    uint control[4] = {
        getTerrainControl(terrain_adjacent_texel_uv[0]),
        getTerrainControl(terrain_adjacent_texel_uv[1]),
        getTerrainControl(terrain_adjacent_texel_uv[2]),
        getTerrainControl(terrain_adjacent_texel_uv[3])
    };

    terrain_control tctrl[4] = {
        decode(control[0]),
        decode(control[1]),
        decode(control[2]),
        decode(control[3])
    };

    terrain_data data[4] = {
        getTerrainData(chunk_uv, tctrl[0]),
        getTerrainData(chunk_uv, tctrl[1]),
        getTerrainData(chunk_uv, tctrl[2]),
        getTerrainData(chunk_uv, tctrl[3]),
    };

    vec2 weights1 = clamp(fract(terrain_texel_uv), 0, 1);
    vec2 weights0 = vec2(1.0) - weights1;

    // Adjust final weights by texture's height/depth + noise. 1 lookup
    float noise3 = texture(terrain_noise_texture, terrain_uv * terrain_noise1_scale).r;

    vec4 weights = vec4(
        blend_weights(weights0.x * weights0.y, clamp(/*mat[0].alb_ht.a*/ noise3, 0.0, 1.0)),
        blend_weights(weights1.x * weights0.y, clamp(/*mat[1].alb_ht.a*/ noise3, 0.0, 1.0)),
        blend_weights(weights0.x * weights1.y, clamp(/*mat[2].alb_ht.a*/ noise3, 0.0, 1.0)),
        blend_weights(weights1.x * weights1.y, clamp(/*mat[3].alb_ht.a*/ noise3, 0.0, 1.0))
    );

    float weight_sum = weights.x + weights.y + weights.z + weights.w;
    float weight_inv = 1.0 / weight_sum;

    vec3 albedo = weight_inv * (
        data[0].albedo * weights.x +
        data[1].albedo * weights.y +
        data[2].albedo * weights.z +
        data[3].albedo * weights.w
    );

    vec3 normal = weight_inv * (
        data[0].normal * weights.x +
        data[1].normal * weights.y +
        data[2].normal * weights.z +
        data[3].normal * weights.w
    );

    vec3 orm = weight_inv * (
        data[0].orm * weights.x +
        data[1].orm * weights.y +
        data[2].orm * weights.z +
        data[3].orm * weights.w
    );

    float vertex_camera_distance = length(getVertexPosition().xz - getCameraPosition().xz);

    vec3 macro_variation = vec3(1.0);
    if (bool(terrain_macro_variation)) {
        float noise1 = texture(
            terrain_noise_texture,
            rotate(chunk_uv * terrain_noise2_scale * .1, cos(terrain_noise2_angle), sin(terrain_noise2_angle)) +
            terrain_noise2_offset).r;
        float noise2 = texture(terrain_noise_texture, chunk_uv * terrain_noise3_scale * .1).r;
        vec3 macrov = mix(terrain_macro_variation1, vec3(1.0), clamp(noise1 + vertex_camera_distance * 0.0002, 0.0, 1.0));
        macrov *= mix(terrain_macro_variation2, vec3(1.0), clamp(noise2 + vertex_camera_distance * 0.0002, 0.0, 1.0));
    }

    mctx.color.xyz = albedo * macro_variation;

    mctx.ao = orm.r;
    mctx.roughness = orm.g;
    mctx.metallic = orm.b;

    {
//        if (vertex_camera_distance <= terrain_vertex_normals_distance) {
        mctx.vertex_normal = getTerrainNormal(terrain_uv);

        #if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
            mat3 TBN = getTerrainTBN(mctx.vertex_normal);
            mctx.tbn_t = TBN[0];
            mctx.tbn_b = TBN[1];
            mctx.tbn_n = TBN[2];
            mctx.normal = normalize(normal);
        #endif
//        }
    }

    if (bool(terrain_show_terrain_size)) {
        if (is_on_tile_border(vertex_position, vec3(terrain_size * terrain_vertex_spacing), 0.1)) {
            mctx.color.xyz = vec3(1.0, 0.0, 0.0);
        }
    }

    if (bool(terrain_show_tiles)) {
        if (is_on_tile_border(vertex_position, vec3(terrain_vertex_spacing), 0.01)) {
            mctx.color.xyz = vec3(0.0, 0.0, 0.0);
        }
    }

    if (bool(terrain_show_texture_chunks)) {
        mctx.color.xyz = vec3(chunk_uv, 0.0);
    }

    if (bool(terrain_show_vertex_normals_distance)) {
        if (vertex_camera_distance <= terrain_vertex_normals_distance) {
            mctx.color.xyz = vec3(0.0, 1.0, 0.0);
        } else {
            mctx.color.xyz = vec3(1.0, 0.0, 0.0);
        }
    }
}