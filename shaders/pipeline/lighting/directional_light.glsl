#include "./directional_csm.glsl"

vec3 getGlobalLightDirectLighting(lighting_context context) {
    DirectionalLight dir_light = getDirectionalLight();
    vec3 L = normalize(-dir_light.direction.xyz);
    vec3 H = normalize(context.V + L);

    float NoL = max(dot(context.N, L), 0.0);
    float NoH = max(dot(context.N, H), 0.0);
    float VoH = clamp(dot(H, context.V), 0.0, 1.0);

    vec3 F = SpecularF_Schlick(context.F0, VoH);

    vec3 radiance = dir_light.color.rgb * dir_light.color.a;

    vec3 diffuse = Diffuse_BRDF(context.albedo, F, context.metallic);
    vec3 specular = Specular_BRDF(context.NoV, NoL, NoH, context.a2, context.roughness, F);

    vec3 lighting = (diffuse + specular) * radiance * NoL;

    #if defined (DIRECTIONAL_CSM)
        float shadow = getDirectionalShadow(context.N, context.world_position);
        lighting *= (1.0 - shadow);
    #endif

    return lighting;
}
