struct lighting_context {
    vec3 albedo;
    float metallic;
    vec3 world_position;
    float roughness;
    vec3 N;
    float a2;
    vec3 V;
    float NoV;
    vec3 F0;
};

lighting_context init_lighting_context(vec3 N, vec3 albedo, vec3 world_position, float metallic, float roughness) {
    lighting_context context;
    context.albedo = albedo;
    context.metallic = metallic;
    context.world_position = world_position;
    context.roughness = roughness;
    context.N = N;
    context.a2 = (roughness * roughness) * (roughness * roughness);
    context.V = normalize(getCameraPosition() - world_position);
    context.NoV = max(dot(N, context.V), 0.0);
    context.F0 = mix(vec3(0.04), albedo, metallic);
    return context;
}
