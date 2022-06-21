struct SpotLight {
    vec4 position;
    vec4 direction;
    vec4 color;
    float cutoff;
    float radius;
};

layout (std140) buffer SPOT_LIGHTS_BUFFER {
    SpotLight _spot_lights[];
};

vec3 computeSpotLight(const LightingContext context, const SpotLight light) {
    vec3 L = normalize(light.position.xyz - context.worldPos);
    float factor = dot(L, -light.direction.xyz);

    vec3 lighting = vec3(0.0);
    if (factor > light.cutoff) {
        vec3 H = normalize(context.V + L);

        float NoL = saturate(dot(context.N, L));
        float NoH = saturate(dot(context.N, H));
        float LoH = saturate(dot(L, H));

        vec3 Fd = DiffuseBRDF(context.diffuseColor, context.roughness, context.NoV, NoL, LoH);
        vec3 Fr = SpecularBRDF(context.roughness, NoH, context.NoV, NoL, LoH, H, context.F0, context.a2);

        vec3 color = Fd + Fr;

        float distance = length(light.position.xyz - context.worldPos);
        float attenuation = 1.0 / (distance * distance);

        lighting = (color * light.color.rgb) * (light.color.a * attenuation * NoL * factor);
    }

    return lighting;
}

vec3 computeSpotLight(const LightingContext context) {
    vec3 direct_light = vec3(0.0);
    for (uint i = 0u; i < getSpotLightsCount(); ++i) {
        float l = length(_spot_lights[i].position.xyz - context.worldPos);
        if (l <= _spot_lights[i].radius) {
            direct_light += computeSpotLight(context, _spot_lights[i]);
        }
    }
    return direct_light;
}
