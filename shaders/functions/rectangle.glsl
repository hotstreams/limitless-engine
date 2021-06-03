float Unity_Rectangle_float(vec2 uv, float w, float h) {
    vec2 d = abs(uv * 2.0 - 1.0) - vec2(w, h);
    d = 1.0 - d / fwidth(d);
    return clamp(min(d.x, d.y), 0.0, 1.0);
}