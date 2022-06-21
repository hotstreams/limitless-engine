vec3 getAmbientLighting(vec3 albedo, float ao) {
    vec4 ambient = getAmbientColor();
    return (ambient.rgb * albedo) * (ambient.a * ao);
}