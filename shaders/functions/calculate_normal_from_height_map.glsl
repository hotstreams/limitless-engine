vec3 calculateNormalFromHeightMap(
    const sampler2D height_map,
    const vec2 uv,
    const float texel_size,
    const float height_scale) {
    float hL = texture(height_map, uv + vec2(-texel_size, 0.0)).r * height_scale;
    float hR = texture(height_map, uv + vec2(texel_size, 0.0)).r * height_scale;
    float hD = texture(height_map, uv + vec2(0.0, -texel_size)).r * height_scale;
    float hU = texture(height_map, uv + vec2(0.0, texel_size)).r * height_scale;

    float dX = hR - hL;
    float dZ = hU - hD;

    return normalize(vec3(-dX, 1.0, -dZ));
}