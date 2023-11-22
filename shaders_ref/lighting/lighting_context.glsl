#include "../functions/common.glsl"
#include "../shading/shading_context.glsl"
#include "./light.glsl"

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
    float distanceSquare = dot(L, L);
    float factor = distanceSquare * light.falloff;
    float smoothFactor = saturate(1.0 - factor * factor);
    float attenuation = smoothFactor * smoothFactor;

    attenuation = attenuation / max(distanceSquare, 1e-4);

    if (light.type == LIGHT_TYPE_SPOT) {
        float cd = dot(-light.direction.xyz, L);
        float att = saturate(cd * light.scale_offset.x + light.scale_offset.y);
        float att2 =  att * att;
        attenuation *= att2;
    }

    return attenuation;
}

LightingContext computeLightingContext(const ShadingContext sctx, const Light light) {
    LightingContext lctx;

    lctx.attenuation = getAttenuation(sctx, light);

    lctx.visibility = 1.0;

    if (light.type == LIGHT_TYPE_DIRECTIONAL) {
        lctx.L = normalize(-light.direction.xyz);
    } else {
        lctx.L = normalize(light.position.xyz - sctx.worldPos);
    }

    lctx.H = normalize(sctx.V + lctx.L);
    lctx.len = length(light.position.xyz - sctx.worldPos);

    lctx.NoL = saturate(dot(sctx.N, lctx.L));
    lctx.NoH = saturate(dot(sctx.N, lctx.H));
    lctx.LoH = saturate(dot(lctx.L, lctx.H));

#if defined (ENGINE_MATERIAL_SPECULAR_GGX)
    lctx.lambdaV = lctx.NoL * sqrt((sctx.NoV - sctx.a2 * sctx.NoV) * sctx.NoV + sctx.a2);
#endif

    return lctx;
}

