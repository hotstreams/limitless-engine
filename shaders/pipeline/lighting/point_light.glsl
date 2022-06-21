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

vec3 computePointLight(const LightingContext context, const PointLight light) {
    vec3 L = normalize(light.position.xyz - context.worldPos);
    vec3 H = normalize(context.V + L);

    float NoL = saturate(dot(context.N, L));
    float NoH = saturate(dot(context.N, H));
    float LoH = saturate(dot(L, H));

    vec3 Fd = DiffuseBRDF(context.diffuseColor, context.roughness, context.NoV, NoL, LoH);
    vec3 Fr = SpecularBRDF(context.roughness, NoH, context.NoV, NoL, LoH, H, context.F0, context.a2);

    vec3 color = Fd + Fr;

    float distance = length(light.position.xyz - context.worldPos);
    float attenuation = 1.0 / (distance * distance * light.quadratic);

    float visibility = 1.0;
    return (color * light.color.rgb) * (light.color.a * NoL * visibility * attenuation);
}

vec3 computePointLight(const LightingContext context) {
    vec3 pointLight = vec3(0.0);
    for (uint i = 0u; i < getPointLightsCount(); ++i) {
        float l = length(_point_lights[i].position.xyz - context.worldPos);
        if (l <= _point_lights[i].radius) {
            pointLight += computePointLight(context, _point_lights[i]);
        }
    }
    return pointLight;
}
