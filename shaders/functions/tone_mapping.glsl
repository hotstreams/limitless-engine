vec3 toneMapping(vec3 color, float exposure) {
    return vec3(1.0) - exp(-color * exposure);
}
