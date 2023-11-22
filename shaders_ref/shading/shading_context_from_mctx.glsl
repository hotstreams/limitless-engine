#include "./shading_context.glsl"
#include "../material/material_context.glsl"

ShadingContext computeShadingContext(const MaterialContext mctx) {
    vec4 color = computeMaterialColor(mctx);
    vec3 normal = computeMaterialNormal(mctx);
    float ao = computeMaterialAO(mctx);
    vec3 emissive_color = computeMaterialEmissiveColor(mctx);

    return computeShadingContext(
        color.rgb,
        color.a,
        mctx.metallic,
        getVertexPosition(),
        mctx.roughness,
        normal,
        ao,
        emissive_color,
        mctx.shading_model
    );
}