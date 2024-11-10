#define _2xSqrt3 3.46410161514

vec2 hashUv( vec2 p)
{
    return fract( sin (( p ) * mat2 (127.1 , 311.7 , 269.5 , 183.3)) *43758.5453) ;
}

void triangleGrid(vec2 uv, out float w1, out float w2, out float w3,  out ivec2 v1, out ivec2 v2, out ivec2 v3)
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

vec3 stochastic_texture(vec2 uv, float index, sampler2DArray s)
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