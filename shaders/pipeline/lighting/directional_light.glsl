#include "./directional_csm.glsl"

vec3 computeDirectionalLight(const LightingContext context) {
    DirectionalLight light = getDirectionalLight();

    vec3 L = normalize(-light.direction.xyz);
    vec3 H = normalize(context.V + L);

    float NoL = saturate(dot(context.N, L));
    float NoH = saturate(dot(context.N, H));
    float LoH = saturate(dot(L, H));

    vec3 Fd = DiffuseBRDF(context.diffuseColor, context.roughness, context.NoV, NoL, LoH);
    vec3 Fr = SpecularBRDF(context.roughness, NoH, context.NoV, NoL, LoH, H, context.F0, context.a2);

    vec3 color = Fd + Fr;

    float visibility = 1.0;

    #if defined (DIRECTIONAL_CSM)
        float shadow = getDirectionalShadow(context.N, context.worldPos);
        visibility *= (1.0 - shadow);
    #endif

    #if defined (MICRO_SHADOWING)
        visibility *= computeMicroShadowing(NoL, context.ambientOcclusion);
    #endif

    return (color * light.color.rgb) * (light.color.a * NoL * visibility);
}
