#include "../lighting/lighting.glsl"

#include "./shading_context_from_gctx.glsl"
#include "../lighting/ambient.glsl"

/* [deferred pipeline] */
vec3 shadeFragment(const GBufferContext gctx) {
    const ShadingContext sctx = computeShadingContext(gctx);

    vec3 color = computeAmbientLighting(sctx.diffuseColor, sctx.ambientOcclusion);

    // if not unlit shading
    if (sctx.shading_model != ENGINE_SHADING_UNLIT) {
        color += computeLights(sctx);
    }

    color.rgb += sctx.emissive_color;

    return color;
}
