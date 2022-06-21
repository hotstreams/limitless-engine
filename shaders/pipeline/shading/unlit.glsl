vec3 unlitShading(vec3 albedo, float ao) {
    return getAmbientLighting(albedo, ao);
}