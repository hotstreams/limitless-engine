vec2 polar_coordinates(vec2 uv, vec2 center, float radial_scale, float length_scale) {
    vec2 delta = uv - center;
    float radius = length(delta) * 2.0 * radial_scale;
    float angle = atan2(delta.x, delta.y) * 1.0 / 6.28 * radial_scale;
    return vec2(radius, angle);
}
