#include "./lighting_context.glsl"
#include "../pipeline/scene.glsl"

#include "../shading/regular.glsl"
#include "../shading/custom.glsl"

#include "./scene_lighting.glsl"
#include "./shadows.glsl"

vec3 computeLight(const ShadingContext sctx, const LightingContext lctx, const Light light) {
    /* [forward pipeline] */
#if defined (ENGINE_MATERIAL_SHADING_REGULAR_MODEL)
    return regularShading(sctx, lctx, light);
#elif defined (ENGINE_MATERIAL_SHADING_CUSTOM_MODEL)
    return customShading(sctx, lctx, light);
#else
    /* [deferred pipeline] */
    switch (sctx.shading_model) {
        case ENGINE_SHADING_LIT: // Lit
            return regularShading(sctx, lctx, light);
        default:
            // deferred pipeline does not support custom shading
            // and unlit paths calculated before
            return vec3(322.0, 0.0, 0.0);
    }
#endif
}

vec3 computeDirectionalLight(const ShadingContext sctx) {
    Light light = getDirectionalLight();
    LightingContext lctx = computeLightingContext(sctx, light);
    lctx.attenuation = 1.0;

    vec3 color = vec3(0.0);

    if (lctx.NoL <= 0.0) {
        return color;
    }

    #if defined (ENGINE_SETTINGS_CSM)
        float shadow = getDirectionalShadow(sctx.N, sctx.worldPos);
        lctx.visibility *= (1.0 - shadow);
    #endif

    #if defined (ENGINE_SETTINGS_MICRO_SHADOWING)
        lctx.visibility *= computeMicroShadowing(lctx.NoL, sctx.ambientOcclusion);
    #endif

    if (lctx.visibility <= 0.0) {
       return color;
    }

    return computeLight(sctx, lctx, light);
}

vec3 computeLights(const ShadingContext sctx) {
    vec3 color = computeDirectionalLight(sctx);

#if defined (ENGINE_SETTINGS_SSR)
    color += sctx.indirect_lighting;
#endif

    for (uint i = 0u; i < getLightCount(); ++i) {
        Light light = getLight(i);

        LightingContext lctx = computeLightingContext(sctx, light);

        if (lctx.NoL <= 0.0 || lctx.attenuation <= 0.0) {
            continue;
        }

        color += computeLight(sctx, lctx, light);
    }

    return color;
}