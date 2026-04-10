#include "../lighting/lighting.glsl"

#include "./shading_context_from_gctx.glsl"
#include "../lighting/ambient.glsl"

/* [deferred pipeline] */
vec3 shadeFragment(const GBufferContext gctx) {
    ShadingContext sctx = computeShadingContext(gctx);

    vec3 color = computeAmbientLighting(sctx.diffuseColor, sctx.ambientOcclusion);

    // if not unlit shading
    if (sctx.shading_model != ENGINE_SHADING_UNLIT) {
        color += computeLights(sctx);
    }

    color.rgb += sctx.emissive_color;

#if defined(ENGINE_SETTINGS_DEBUG_LIGHT_TILES)
    // Visualize per-tile light list size (blue -> green -> red).
    float c = float(getCurrentTileLightCount());
    float t = clamp(c / 32.0, 0.0, 1.0);
    vec3 overlay = vec3(t, 1.0 - abs(t * 2.0 - 1.0), 1.0 - t);
    color = mix(color, overlay, 0.45);
#endif

    return color;
}
