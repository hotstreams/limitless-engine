#include "../lighting/lighting.glsl"

#include "./shading_context_from_mctx.glsl"
#include "../lighting/refraction.glsl"
#include "../lighting/ambient.glsl"

/* [forward pipeline] */
vec4 shadeFragment(const MaterialContext mctx) {
    ShadingContext sctx = computeShadingContext(mctx);

#if defined (ENGINE_MATERIAL_SHADING_CUSTOM_MODEL)
    return customShading(sctx);
#else
    vec3 ambient = computeAmbientLighting(sctx.diffuseColor, sctx.ambientOcclusion);
    vec4 color = vec4(ambient, sctx.alpha);

#if !defined (ENGINE_MATERIAL_SHADING_UNLIT_MODEL)
    color.rgb += computeLights(sctx);
#endif

#if defined (ENGINE_MATERIAL_REFRACTION)
    color.rgb += computeRefraction(sctx, mctx.IoR, mctx.absorption);
#endif

    color.rgb += sctx.emissive_color;
#endif

    return color;
}
