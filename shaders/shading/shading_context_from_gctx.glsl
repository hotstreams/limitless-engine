#include "./shading_context.glsl"
#include "../pipeline/gbuffer_context.glsl"
#include "../lighting/ssao.glsl"
#include "../lighting/ssr.glsl"

ShadingContext computeShadingContext(const GBufferContext gctx) {
    float ao = gctx.ao;

#if defined (ENGINE_SETTINGS_SSAO)
    ao = min(ao, getSSAO(uv));
#endif

    vec3 indirect_lighting = vec3(0.0);
#if defined (ENGINE_SETTINGS_SSR)
    indirect_lighting = getSSR(uv);
#endif

    return computeShadingContext(
        gctx.color,
        1.0, // deferred shading, no alpha channel
        gctx.metallic,
        gctx.position,
        gctx.roughness,
        gctx.normal,
        ao,
        gctx.emissive_color,
        gctx.shading_model,
        indirect_lighting
    );
}
