vec2 _hash_uv(vec2 p) {
    return fract(sin((p) * mat2(127.1, 311.7, 269.5, 183.3)) * 43758.5453);
}

void _triangle_grid(vec2 uv, out float w1, out float w2, out float w3,  out ivec2 v1, out ivec2 v2, out ivec2 v3) {
    const mat2 skewedGridTransform = mat2(1.0, 0.0, -0.57735027, 1.15470054);
    vec2 skewedUv = skewedGridTransform * uv * 3.46410161514;

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

void _get_stochastic_data(
    vec2 uv,
    out vec2 uv1,
    out vec2 uv2,
    out vec2 uv3,
    out vec4 duvd,
    out vec3 weights
) {
    ivec2 v1, v2, v3;
    _triangle_grid(uv, weights.x, weights.y, weights.z, v1, v2, v3);

    uv1 = uv + _hash_uv(v1);
    uv2 = uv + _hash_uv(v2);
    uv3 = uv + _hash_uv(v3);

    duvd.xy = dFdx(uv);
    duvd.zw = dFdy(uv);
}

vec3 get_detiled_texture(
    vec2 uv1,
    vec2 uv2,
    vec2 uv3,
    vec4 duvd,
    vec3 weights,
    sampler2DArray tex,
    float index
) {
    vec3 T1 = textureGrad(tex, vec3(uv1, index), duvd.xy, duvd.zw).rgb;
    vec3 T2 = textureGrad(tex, vec3(uv2, index), duvd.xy, duvd.zw).rgb;
    vec3 T3 = textureGrad(tex, vec3(uv3, index), duvd.xy, duvd.zw).rgb;

    return weights.x * T1 + weights.y * T2 + weights.z * T3;
}

vec3 get_detiled_texture(sampler2DArray tex, vec2 uv, float index) {
    vec2 uv1, uv2, uv3;
    vec3 weights;
    vec4 duvd;
    _get_stochastic_data(uv, uv1, uv2, uv3, duvd, weights);

    vec3 T1 = textureGrad(tex, vec3(uv1, index), duvd.xy, duvd.zw).rgb;
    vec3 T2 = textureGrad(tex, vec3(uv2, index), duvd.xy, duvd.zw).rgb;
    vec3 T3 = textureGrad(tex, vec3(uv3, index), duvd.xy, duvd.zw).rgb;

    return weights.x * T1 + weights.y * T2 + weights.z * T3;
}