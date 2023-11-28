float cheap_contrast(vec3 color, vec3 contrast) {
    return clamp(mix(0.0 - contrast, contrast + 1.0, color), 0.0, 1.0).r;
}