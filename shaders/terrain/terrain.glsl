#include "../functions/is_on_tile_border.glsl"
#include "../functions/stochastic.glsl"
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

    if (tctrl.blend > 0.0) {
        vec3 extra = stochastic_texture(uv, tctrl.extra_id, terrain_albedo_texture);

        data.albedo = height_blend(data.albedo, 1.0, extra, 1.0, tctrl.blend);
        data.normal = height_blend(data.normal, 1.0, extra, 1.0, tctrl.blend);
        data.orm = height_blend(data.orm, 1.0, extra, 1.0, tctrl.blend);
    }

    return data;
}

void calculateTerrain(inout MaterialContext mctx) {
    vec3 v_vertex = getVertexPosition();

    vec2 texel_uv = get_region_uv(v_vertex.xz * terrain_vertex_density);
    vec2 uv = texel_uv / terrain_size * terrain_texture_scale;

    vec2 texel_pos_floor = floor(texel_uv);

    vec4 mirror = vec4(fract(texel_pos_floor * 0.5) * 2.0, 1.0, 1.0);
    mirror.zw = vec2(1.0) - mirror.xy;

    vec2 indexUV[4] = {
        get_region_uv(texel_pos_floor + mirror.xy),
        get_region_uv(texel_pos_floor + mirror.xw),
        get_region_uv(texel_pos_floor + mirror.zy),
        get_region_uv(texel_pos_floor + mirror.zw)
    };

    vec2 indexUV1[4] = {
        uv + mirror.xy / terrain_size,
        uv + mirror.xw / terrain_size,
        uv + mirror.zy / terrain_size,
        uv + mirror.zw / terrain_size
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
    vec2 weights1 = clamp(texel_uv - texel_pos_floor, 0, 1);
    weights1 = mix(weights1, vec2(1.0) - weights1, mirror.xy);
    vec2 weights0 = vec2(1.0) - weights1;

    // Adjust final weights by texture's height/depth + noise. 1 lookup
    float noise3 = texture(terrain_noise_texture, uv * terrain_noise1_scale).r;
    vec4 weights = vec4(
        blend_weights(weights0.x * weights0.y, clamp(/*mat[0].alb_ht.a*/ + noise3, 0.0, 1.0)),
        blend_weights(weights0.x * weights1.y, clamp(/*mat[1].alb_ht.a*/ + noise3, 0.0, 1.0)),
        blend_weights(weights1.x * weights0.y, clamp(/*mat[2].alb_ht.a*/ + noise3, 0.0, 1.0)),
        blend_weights(weights1.x * weights1.y, clamp(/*mat[3].alb_ht.a*/ + noise3, 0.0, 1.0))
    );

    float weight_sum = dot(weights, vec4(1.0));
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

    float v_vertex_xz_dist = length(v_vertex.xz - getCameraPosition().xz);

	float noise1 = texture(terrain_noise_texture,
                           rotate(uv * terrain_noise2_scale * .1, cos(terrain_noise2_angle), sin(terrain_noise2_angle)) +
                           terrain_noise2_offset).r;
	float noise2 = texture(terrain_noise_texture, uv * terrain_noise3_scale * .1).r;
	vec3 macrov = mix(terrain_macro_variation1, vec3(1.0), clamp(noise1 + v_vertex_xz_dist * 0.0002, 0.0, 1.0));
	macrov *= mix(terrain_macro_variation2, vec3(1.0), clamp(noise2 + v_vertex_xz_dist * 0.0002, 0.0, 1.0));

    mctx.color.xyz = albedo;// * macrov;
    mctx.normal = normal;
    mctx.ao = orm.r;
    mctx.roughness = orm.g;
    mctx.metallic = orm.b;

//    if (is_on_tile_border(v_vertex, vec3(64.0), 0.1)) {
//        mctx.color.xyz = vec3(1.0, 0.0, 0.0);
//    }
}