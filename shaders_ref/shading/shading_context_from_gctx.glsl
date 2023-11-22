#include "./shading_context.glsl"
#include "../pipeline/gbuffer_context.glsl"

ShadingContext computeShadingContext(const GBufferContext gctx) {
    return computeShadingContext(
        gctx.color,
        1.0, // deferred shading, no alpha channel
        gctx.metallic,
        gctx.position,
        gctx.roughness,
        gctx.normal,
        gctx.ao,
        gctx.emissive_color,
        gctx.shading_model
    );
}
