vec2 radial_shear(vec2 uv, vec2 center, float strength, vec2 offset) {
    vec2 delta = uv - center;
    float delta2 = dot(delta.xy, delta.xy);
    vec2 delta_offset = vec2(delta2 * strength);
    return uv + vec2(delta.y, -delta.x) * delta_offset + offset;
}
