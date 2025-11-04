#include "../functions/brdf.glsl"
#include "../functions/brdf_ctx.glsl"

/**
 *  Computes color for lit material with regular PBR shading model.
*/
vec3 regularShading(const ShadingContext sctx, const LightingContext lctx, const Light light) {
    vec3 Fd = DiffuseBRDF(sctx, lctx);
    vec3 Fr = SpecularBRDF(sctx, lctx);

    // Use pre-computed energy compensation (computed once per pixel)
    vec3 color = Fd + Fr * sctx.energyCompensation;

//#if defined (ENGINE_MATERIAL_REFRACTION)
//    Fd *= (1.0 - sctx.transmission);
//#endif

    return (color * light.color.rgb) * (light.color.a * lctx.NoL * lctx.attenuation * lctx.visibility);
}
