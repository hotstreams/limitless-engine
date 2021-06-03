void rotate_uv(vec2 uv, vec2 center, float rotation) {
    uv -= center;
    float s = sin(rotation);
    float c = cos(rotation);
    mat2 rMatrix = mat2(c, -s, s, c);
    rMatrix *= 0.5;
    rMatrix += 0.5;
    rMatrix = rMatrix * 2.0 - 1.0;
    uv.xy = mul(uv.xy, rMatrix);
    uv += center;
    return UV;
}