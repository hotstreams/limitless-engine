#include "../functions/is_on_tile_border.glsl"
#include "../functions/stochastic.glsl"
#include "../functions/noise.glsl"
#include "./terrain_control.glsl"
#include "../instance/instance_fs.glsl"

vec2 get_region_uv(const vec2 uv) {
    return vec2(mod(uv, terrain_size));
}

uint getTerrainControl(vec2 uv) {
    return texelFetch(terrain_control_texture, ivec2(uv), 0).r;
}

vec3 getTerrainAlbedo(vec3 uv) {
    return texture(terrain_albedo_texture, uv).rgb;
}

float blend_weights(float weight, float detail) {
    weight = smoothstep(0.0, 1.0, weight);
    weight = sqrt(weight * 0.5);
    float result = max(0.1 * weight, 10.0 * (weight + detail) + 1.0f - (detail + 10.0));
    return result;
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

terrain_data getTerrainData(vec2 uv, const terrain_control tctrl) {
    terrain_data data;

    data.albedo = stochastic_texture(uv, tctrl.base_id, terrain_albedo_texture);
    data.normal = stochastic_texture(uv, tctrl.base_id, terrain_normal_texture);
    data.orm = stochastic_texture(uv, tctrl.base_id, terrain_orm_texture);

//    data.albedo = texture(terrain_albedo_texture, vec3(uv, tctrl.base_id)).rgb;
//    data.normal = texture(terrain_normal_texture, vec3(uv, tctrl.base_id)).rgb;
//    data.orm = texture(terrain_orm_texture, vec3(uv, tctrl.base_id)).rgb;

    if (tctrl.blend > 0.0) {
        vec3 extra = stochastic_texture(uv, tctrl.extra_id, terrain_albedo_texture);

        data.albedo = height_blend(data.albedo, 1.0, extra, 1.0, tctrl.blend);
        data.normal = height_blend(data.normal, 1.0, extra, 1.0, tctrl.blend);
        data.orm = height_blend(data.orm, 1.0, extra, 1.0, tctrl.blend);
    }

    return data;
}

float get_height(vec2 uv)
{
    float height = texture(terrain_height_texture, uv).r * terrain_height_scale;
    height += value_noise(uv);
    return height;
}

vec3 getTerrainNormal(vec2 uv) {
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

// returns texel UV [0, world space terrain size] in terrain space
vec2 getTerrainTexelUV(vec2 vertex_position) {
    return mod(vertex_position, terrain_size * terrain_vertex_spacing);
}

// returns UV [0, 1] in terrain space
vec2 getTerrainUV(vec2 texel_uv) {
    return texel_uv / (terrain_size * terrain_vertex_spacing);
}

// returns UV [0, 1] in chunk space
vec2 getChunkUV(vec2 uv) {
    return uv * terrain_texture_scale;
}

void calculateTerrain(inout MaterialContext mctx) {
    vec3 vertex_position = getVertexPosition();
    vec2 terrain_texel_uv = getTerrainTexelUV(vertex_position.xz);
    vec2 terrain_uv = getTerrainUV(terrain_texel_uv);
    vec2 chunk_uv = getChunkUV(terrain_uv);

    vec2 texel_pos_floor = floor(terrain_texel_uv);
    vec4 mirror = vec4(fract(texel_pos_floor * 0.5) * 2.0, 1.0, 1.0);
    mirror.zw = vec2(1.0) - mirror.xy;

    vec2 indexUV[4] = {
        get_region_uv(texel_pos_floor + mirror.xy),
        get_region_uv(texel_pos_floor + mirror.xw),
        get_region_uv(texel_pos_floor + mirror.zy),
        get_region_uv(texel_pos_floor + mirror.zw)
    };

    vec2 indexUV1[4] = {
        chunk_uv + mirror.xy / (terrain_size * terrain_vertex_spacing),
        chunk_uv + mirror.xw / (terrain_size * terrain_vertex_spacing),
        chunk_uv + mirror.zy / (terrain_size * terrain_vertex_spacing),
        chunk_uv + mirror.zw / (terrain_size * terrain_vertex_spacing)
    };

    uint control[4] = {
        getTerrainControl(indexUV[0]),
        getTerrainControl(indexUV[1]),
        getTerrainControl(indexUV[2]),
        getTerrainControl(indexUV[3])
    };

    terrain_control tctrl[4] = {
        decode(control[0]),
        decode(control[1]),
        decode(control[2]),
        decode(control[3])
    };

    terrain_data data[4] = {
        getTerrainData(indexUV1[0], tctrl[0]),
        getTerrainData(indexUV1[1], tctrl[1]),
        getTerrainData(indexUV1[2], tctrl[2]),
        getTerrainData(indexUV1[3], tctrl[3]),
    };

    // Calculate weight for the pixel position between the vertices
    // Bilinear interpolation of difference of uv and floor(uv)
    vec2 weights1 = clamp(terrain_texel_uv - texel_pos_floor, 0, 1);
    weights1 = mix(weights1, vec2(1.0) - weights1, mirror.xy);
    vec2 weights0 = vec2(1.0) - weights1;

    // Adjust final weights by texture's height/depth + noise. 1 lookup
    float noise3 = texture(terrain_noise_texture, chunk_uv * terrain_noise1_scale).r;

    vec4 weights = vec4(
        blend_weights(weights0.x * weights0.y, clamp(/*mat[0].alb_ht.a*/ noise3, 0.0, 1.0)),
        blend_weights(weights0.x * weights1.y, clamp(/*mat[1].alb_ht.a*/ noise3, 0.0, 1.0)),
        blend_weights(weights1.x * weights0.y, clamp(/*mat[2].alb_ht.a*/ noise3, 0.0, 1.0)),
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

    float v_vertex_xz_dist = length(getVertexPosition().xz - getCameraPosition().xz);

	float noise1 = texture(
       terrain_noise_texture,
       rotate(chunk_uv * terrain_noise2_scale * .1, cos(terrain_noise2_angle), sin(terrain_noise2_angle)) +
       terrain_noise2_offset).r;
	float noise2 = texture(terrain_noise_texture, chunk_uv * terrain_noise3_scale * .1).r;
	vec3 macrov = mix(terrain_macro_variation1, vec3(1.0), clamp(noise1 + v_vertex_xz_dist * 0.0002, 0.0, 1.0));
	macrov *= mix(terrain_macro_variation2, vec3(1.0), clamp(noise2 + v_vertex_xz_dist * 0.0002, 0.0, 1.0));

    mctx.color.xyz = albedo * macrov;

    {
        //TODO: based on distance to camera
        mctx.vertex_normal = getTerrainNormal(terrain_uv);

        #if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
            mat3 TBN = getTerrainTBN(mctx.vertex_normal);
            mctx.tbn_t = TBN[0];
            mctx.tbn_b = TBN[1];
            mctx.tbn_n = TBN[2];
            mctx.normal = normalize(normal);
        #endif
    }

//    if (is_on_tile_border(vertex_position, vec3(terrain_size * terrain_vertex_spacing), 0.1)) {
//        mctx.color.xyz = vec3(1.0, 0.0, 0.0);
//    }

//    if (is_on_tile_border(v_vertex, vec3(terrain_vertex_spacing), 0.01)) {
//        mctx.color.xyz = vec3(0.0, 0.0, 0.0);
//    }
}