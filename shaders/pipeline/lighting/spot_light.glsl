struct SpotLight {
    vec4 position;
    vec4 direction;
    vec4 color;
    float cutoff;
};

layout (std140) buffer SPOT_LIGHTS_BUFFER {
    SpotLight _spot_lights[];
};

vec3 getRadiance(SpotLight light, vec3 world_pos) {
    float distance = length(light.position.xyz - world_pos);
    float attenuation = 1.0 / (distance * distance);
    return light.color.rgb * attenuation;
}

vec3 getDirectLighting(lighting_context context, SpotLight light) {
    vec3 L = normalize(light.position.xyz - context.world_position);
    float factor = dot(L, light.direction.xyz);

    vec3 lighting = vec3(0.0);
    if (factor > light.cutoff) {
        vec3 H = normalize(context.V + L);

        float NoL = max(dot(context.N, L), 0.0);
        float NoH = max(dot(context.N, H), 0.0);

        float VoH = clamp(dot(H, context.V), 0.0, 1.0);

        vec3 F = SpecularF_Schlick(context.F0, VoH);

        vec3 radiance = getRadiance(light, context.world_position) * factor;

        vec3 diffuse = Diffuse_BRDF(context.albedo, F, context.metallic);
        vec3 specular = Specular_BRDF(context.NoV, NoL, NoH, context.a2, context.roughness, F);

        lighting = (diffuse + specular) * radiance * NoL;
    }

    return lighting;
}

vec3 getSpotLightDirectLighting(lighting_context context) {
    vec3 direct_light = vec3(0.0);
    for (uint i = 0u; i < getSpotLightsCount(); ++i) {
        direct_light += getDirectLighting(context, _spot_lights[i]);
    }
    return direct_light;
}
