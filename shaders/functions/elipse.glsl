float Unity_Ellipse_float(vec2 uv, float w, float h) {
    float d = length((uv * 2.0 - 1.0) / vec2(w, h));
    return clamp((1.0 - d) / fwidth(d), 0.0, 1.0);
}