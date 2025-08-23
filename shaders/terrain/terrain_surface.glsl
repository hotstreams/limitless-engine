#include "./terrain.glsl"
#include "../functions/stochastic.glsl"
    #define dFdxCoarse(a) dFdx(a)
    #define dFdyCoarse(a) dFdy(a)
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

void calculateTerrain2(inout MaterialContext mctx) {
    vec3 vertex_position = getVertexPosition();
    vec2 terrain_texel_uv = getTerrainTexelUV(vertex_position.xz);
    vec2 terrain_uv = getTerrainUV(terrain_texel_uv);
    vec2 chunk_uv = getChunkUV(terrain_uv);

    vec2 terrain_texel_base = floor(terrain_texel_uv);

	ivec2 index[4];
	const vec3 offsets = vec3(0, 1, 2);

	// control map lookups, used for some normal lookups as well
	index[0] = ivec2(getTerrainTexelUV(terrain_texel_base + offsets.xy));
	index[1] = ivec2(getTerrainTexelUV(terrain_texel_base + offsets.yy));
	index[2] = ivec2(getTerrainTexelUV(terrain_texel_base + offsets.yx));
	index[3] = ivec2(getTerrainTexelUV(terrain_texel_base + offsets.xx));

    // Terrain normals
    vec3 index_normal[4];
    float h[4];
    // allows additional derivatives, eg world noise, brush previews etc
    float u = 0.0;
    float v = 0.0;

  //  u+= value_noise(terrain_uv);
   // v += value_noise(terrain_uv);

    // Re-use index[] for the first lookups, skipping some math. 3 lookups
    h[3] = texelFetch(terrain_height_texture, index[3], 0).r  * terrain_height_scale; // 0 (0,0)
    h[2] = texelFetch(terrain_height_texture, index[2], 0).r  * terrain_height_scale; // 1 (1,0)
    h[0] = texelFetch(terrain_height_texture, index[0], 0).r  * terrain_height_scale; // 2 (0,1)
    index_normal[3] = normalize(vec3(h[3] - h[2] + u, terrain_vertex_spacing, h[3] - h[0] + v));

    // Set flat world normal - overwritten if bilerp is true
    vec3 w_normal = index_normal[3];

    vec3 base_ddx = dFdxCoarse(vertex_position);
    vec3 base_ddy = dFdyCoarse(vertex_position);
    // Calculate the effective mipmap for regionspace, and when less than 0,
    // skip all extra lookups required for bilinear blend.
    float region_mip = log2(max(length(base_ddx.xz), length(base_ddy.xz)));
    bool bilerp = region_mip < 0.0;

if (bilerp)
{
	vec2 index_id = floor(terrain_texel_uv);

    // 5 lookups
    // Fetch the additional required height values for smooth normals
    h[1] = texelFetch(terrain_height_texture, index[1], 0).r   * terrain_height_scale; // 3 (1,1)
    float h_4 = texelFetch(terrain_height_texture, ivec2(getTerrainTexelUV(index_id + offsets.yz)), 0).r   * terrain_height_scale; // 4 (1,2)
    float h_5 = texelFetch(terrain_height_texture, ivec2(getTerrainTexelUV(index_id + offsets.zy)), 0).r   * terrain_height_scale; // 5 (2,1)
    float h_6 = texelFetch(terrain_height_texture, ivec2(getTerrainTexelUV(index_id + offsets.zx)), 0).r   * terrain_height_scale; // 6 (2,0)
    float h_7 = texelFetch(terrain_height_texture, ivec2(getTerrainTexelUV(index_id + offsets.xz)), 0).r  * terrain_height_scale; // 7 (0,2)

    index_normal[0] = normalize(vec3(h[0] - h[1] + u, terrain_vertex_spacing, h[0] - h_7 + v));
    index_normal[1] = normalize(vec3(h[1] - h_5 + u, terrain_vertex_spacing, h[1] - h_4 + v));
    index_normal[2] = normalize(vec3(h[2] - h_6 + u, terrain_vertex_spacing, h[2] - h[1] + v));


	// Lookup offsets, ID and blend weight
	vec2 weight = fract(terrain_texel_uv);
	vec2 invert = 1.0 - weight;
	vec4 weights = vec4(
		invert.x * weight.y, // 0
		weight.x * weight.y, // 1
		weight.x * invert.y, // 2
		invert.x * invert.y  // 3
	);

    // Set interpolated world normal
    w_normal =
        index_normal[0] * weights[0] +
        index_normal[1] * weights[1] +
        index_normal[2] * weights[2] +
        index_normal[3] * weights[3] ;
}
    mctx.color = vec4(w_normal * 0.5 + 0.5, 0.0);
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


        {

     vec2 terrain_texel_uv = getTerrainTexelUV(vertex_position.xz);
        vec2 terrain_uv = getTerrainUV(terrain_texel_uv);
        vec2 chunk_uv = getChunkUV(terrain_uv);

        vec2 terrain_texel_base = floor(terrain_texel_uv);

    ivec2 index[4];
    const vec3 offsets = vec3(0, 1, 2);

    // control map lookups, used for some normal lookups as well
    index[0] = ivec2(getTerrainTexelUV(terrain_texel_base + offsets.xy));
    index[1] = ivec2(getTerrainTexelUV(terrain_texel_base + offsets.yy));
    index[2] = ivec2(getTerrainTexelUV(terrain_texel_base + offsets.yx));
    index[3] = ivec2(getTerrainTexelUV(terrain_texel_base + offsets.xx));

    // Terrain normals
    vec3 index_normal[4];
    float h[4];
    // allows additional derivatives, eg world noise, brush previews etc
    float u = 0.0;
    float v = 0.0;

  //  u+= value_noise(terrain_uv);
   // v += value_noise(terrain_uv);

    // Re-use index[] for the first lookups, skipping some math. 3 lookups
    h[3] = texelFetch(terrain_height_texture, index[3], 0).r  * terrain_height_scale; // 0 (0,0)
    h[2] = texelFetch(terrain_height_texture, index[2], 0).r  * terrain_height_scale; // 1 (1,0)
    h[0] = texelFetch(terrain_height_texture, index[0], 0).r  * terrain_height_scale; // 2 (0,1)
    index_normal[3] = normalize(vec3(h[3] - h[2] + u, terrain_vertex_spacing, h[3] - h[0] + v));

    mctx.vertex_normal = index_normal[3];


        }

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

    mctx.color.xyz = mctx.normal * 0.5 + 0.5;
}