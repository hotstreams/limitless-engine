struct PointLight {
    vec4 position;
    vec4 color;
    float constant;
    float linear;
    float quadratic;
    float radius;
};

layout (std140) buffer POINT_LIGHTS_BUFFER {
    PointLight _point_lights[];
};

vec3 getRadiance(PointLight light, vec3 world_pos) {
    float distance = length(light.position.xyz - world_pos);
    float attenuation = 1.0 / (distance * distance * light.quadratic);
    return light.color.rgb * light.color.a * attenuation;
}

vec3 getDirectLighting(lighting_context context, PointLight light) {
    vec3 L = normalize(light.position.xyz - context.world_position);
    vec3 H = normalize(context.V + L);

    float NoL = max(dot(context.N, L), 0.0);
    float NoH = max(dot(context.N, H), 0.0);
    float VoH = clamp(dot(H, context.V), 0.0, 1.0);

    vec3 F = SpecularF_Schlick(context.F0, VoH);

    vec3 radiance = getRadiance(light, context.world_position);

    vec3 diffuse = Diffuse_BRDF(context.albedo, F, context.metallic);
    vec3 specular = Specular_BRDF(context.NoV, NoL, NoH, context.a2, context.roughness, F);

    return (diffuse + specular) * radiance * NoL;
}

vec3 getPointLightDirectLighting(lighting_context context) {
    vec3 direct_light = vec3(0.0);
    for (uint i = 0u; i < getPointLightsCount(); ++i) {
        if (length(_point_lights[i].position.xyz - context.world_position) <= _point_lights[i].radius) {
            direct_light += getDirectLighting(context, _point_lights[i]);
        }
    }
    return direct_light;
}
