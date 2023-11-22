#include "../functions/brdf.glsl"

/**
 *  Computes color for lit material with regular PBR shading model.
*/
vec3 regularShading(const ShadingContext sctx, const LightingContext lctx, const Light light) {
    vec3 Fd = DiffuseBRDF(sctx, lctx);
    vec3 Fr = SpecularBRDF(sctx, lctx);

    vec3 color = Fd + Fr;

//#if defined (ENGINE_MATERIAL_REFRACTION)
//    Fd *= (1.0 - sctx.transmission);
//#endif

    return (color * light.color.rgb) * (light.color.a * lctx.NoL * lctx.attenuation * lctx.visibility);
}
