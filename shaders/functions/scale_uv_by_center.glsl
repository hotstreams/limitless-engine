vec2 scale_uv_by_center(vec2 uv, float scale) {
    return uv / scale + 0.5 - 0.5 / scale;
}