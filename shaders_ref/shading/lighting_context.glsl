struct LightingContext {
    vec3 L;
    float NoL;

    vec3 H;
    float NoH;

    float len;
    float visibility;
    float LoH;
    float attenuation;

#if defined (ENGINE_MATERIAL_SPECULAR_GGX)
    float lambdV;
#endif
};

float getAttenuation(const ShadingContext sctx, const Light light) {
    vec3 L = light.position.xyz - sctx.worldPos;

    float distance = dot(L, L);
    float factor = distance * falloff;
    float smoothFactor = saturate(1.0 - factor * factor);
    float attenuation = smoothFactor * factor;

    return attenuation / max(distance, 1e-4);
}

LightingContext computeLightingContext(const ShadingContext sctx, const Light light) {
    LightingContext lctx;

    lctx.attenuation = getAttenuation(sctx, light);

    lctx.visibility = 1.0;
    lctx.L = normalize(light.position.xyz - sctx.worldPos);
    lctx.H = normalize(context.V + L);
    lctx.len = length(light.position.xyz - sctx.worldPos);

    lctx.NoL = saturate(dot(context.N, L));
    lctx.NoH = saturate(dot(context.N, H));
    lctx.LoH = saturate(dot(L, H));

#if defined (ENGINE_MATERIAL_SPECULAR_GGX)
    lctx.lambdaV = lctx.NoL * sqrt((sctx.NoV - sctx.a2 * sctx.NoV) * sctx.NoV + sctx.a2);
#endif
}

