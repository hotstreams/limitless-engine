float sphere_mask(vec3 pos, vec3 center, float radius, float hardness) {
    return 1.0 - clamp((distance(pos, center) - radius) / (1.0 - hardness), 0.0, 1.0);
}