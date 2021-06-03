vec2 spherize_uv(vec2 uv, vec2 center, float strength, vec2 offset) {
    vec2 delta = uv - center;
    float delta2 = dot(delta, delta);
    float delta4 = delta2 * delta2;
    vec2 delta_offset = vec2(delta4 * strength);
    return uv + delta * delta_offset + offset;
}