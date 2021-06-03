//float sphere_mask(vec3 pos, vec3 center, float radius, float hardness) {
//    float normalized_distance = distance(pos, center) * 1.0 / max(0.000001, radius);
//    float inv_hardness = 1.0 / max(1.0 - hardness, 0.000001);
//    return clamp((1.0 - normalized_distance) * inv_hardness, 0.0, 1.0);
//}

float sphere_mask(vec3 pos, vec3 center, float radius, float hardness) {
    return 1.0 - clamp((distance(pos, center) - radius) / (1.0 - hardness), 0.0, 1.0);
}