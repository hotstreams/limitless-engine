float fresnel(vec3 normal, vec3 view, float power) {
    return pow((1.0 - clamp(dot(normalize(normal), normalize(view)), 0.0, 1.0)), power);
}