float MacroVariation() {
    vec2 uv = getVertexPosition().xz;
    vec2 uv1 = uv * variation1;
    vec2 uv2 = uv * variation2;
    vec2 uv3 = uv * variation3;
    float r1 = texture(_terrain_variation, uv1).r + 0.5;
    float r2 = texture(_terrain_variation, uv2).r + 0.5;
    float r3 = texture(_terrain_variation, uv3).r + 0.5;
    return r1 * r2 * r3;
}

vec3 MacroContrast(float variation) {
    return mix(variation_contrast, vec3(1.0), vec3(variation));
}

#define _2xSqrt3 3.46410161514

vec2 hashUv( vec2 p)
{
    return fract ( sin (( p ) * mat2 (127.1 , 311.7 , 269.5 , 183.3))
                   *43758.5453) ;
}

void triangleGrid(vec2 uv,
out float w1, out float w2, out float w3,
out ivec2 v1, out ivec2 v2, out ivec2 v3)
{
    const mat2 skewedGridTransform = mat2(1.0, 0.0, -0.57735027, 1.15470054);
    vec2 skewedUv = skewedGridTransform * uv * _2xSqrt3;

    ivec2 vBase = ivec2(floor(skewedUv));
    vec3 temp = vec3(fract(skewedUv), 0);
    temp.z = 1.0 - temp.x - temp.y;
    if (temp.z > 0.0) {
        w1 = temp.z;
        w2 = temp.y;
        w3 = temp.x;
        v1 = vBase;
        v2 = vBase + ivec2(0, 1);
        v3 = vBase + ivec2(1, 0);
    } else {
        w1 = -temp.z;
        w2 = 1.0 - temp.y;
        w3 = 1.0 - temp.x;
        v1 = vBase + ivec2(1, 1);
        v2 = vBase + ivec2(1, 0);
        v3 = vBase + ivec2(0, 1);
    }
}

vec3 StochasticTexture(vec2 uv, float index, sampler2DArray s)
{
    float w1 = 0, w2 = 0, w3 = 0;
    ivec2 v1, v2, v3;
    triangleGrid(uv, w1, w2, w3, v1, v2, v3);

    vec2 uv1 = uv + hashUv(v1);
    vec2 uv2 = uv + hashUv(v2);
    vec2 uv3 = uv + hashUv(v3);

    vec2 duvdx = dFdx(uv);
    vec2 duvdy = dFdy(uv);

    vec3 T1 = textureGrad(s, vec3(uv1, index), duvdx, duvdy).rgb;
    vec3 T2 = textureGrad(s, vec3(uv2, index), duvdx, duvdy).rgb;
    vec3 T3 = textureGrad(s, vec3(uv3, index), duvdx, duvdy).rgb;

    vec3 col = w1 * T1 + w2 * T2 + w3 * T3;
    return col;
}

void calculateTerrain(inout MaterialContext mctx) {
    vec2 terrain_uv = (getVertexPosition().xz / 25.6) * 256.0;

    vec3 variation = MacroContrast(MacroVariation());

    vec3 diffuse = StochasticTexture(terrain_uv, getVertexTileCurrent(), _terrain_diffuse_texture);
    vec3 normal = StochasticTexture(terrain_uv, getVertexTileCurrent(), _terrain_normal_texture);
    vec3 orm = StochasticTexture(terrain_uv, getVertexTileCurrent(), _terrain_orm_texture);

    vec2 uv = getVertexUV();

    float vectors[8] = {
    smoothstep(0.0, 1.0, length(uv)),
    smoothstep(0.0, 1.0, uv.y),
    smoothstep(0.0, 1.0, length(vec2(1.0 - uv.x, uv.y))),
    smoothstep(0.0, 1.0, uv.x),
    smoothstep(0.0, 1.0, 1.0 - uv.x),
    smoothstep(0.0, 1.0, length(vec2(uv.x, 1.0 - uv.y))),
    smoothstep(0.0, 1.0, 1.0 - uv.y),
    smoothstep(0.0, 1.0, length(vec2(1.0 - uv.x, 1.0 - uv.y)))
    };

    uint mask[8];
    unpack8x4(getVertexColor(), mask);

    uint types[8];
    unpack8x4(getVertexTileType(), types);

    // iterate for all adjacent tiles
    for (uint i = 0u; i < 8u; ++i) {
        // shoud we blend
        if (mask[i] == 1u) {
            // fetch adjacent type
            vec3 adjacent_diffuse = StochasticTexture(terrain_uv, types[i], _terrain_diffuse_texture).rgb;
            vec3 adjacent_normal = StochasticTexture(terrain_uv, types[i], _terrain_normal_texture).xyz;
            vec3 adjacent_orm = StochasticTexture(terrain_uv, types[i], _terrain_orm_texture).rgb;

            // mix
            diffuse = mix(diffuse, adjacent_diffuse, (1.0 - vectors[i]));
            normal = mix(normal, adjacent_normal, (1.0 - vectors[i]));
            orm = mix(orm, adjacent_orm, (1.0 - vectors[i]));
        }
    }

//    diffuse *= variation;

    // store result
    mctx.color.rgb = diffuse;
    mctx.normal = normalize(normal);
    mctx.ao = orm.r;
    mctx.roughness = orm.g;
    mctx.metallic = orm.b;
}