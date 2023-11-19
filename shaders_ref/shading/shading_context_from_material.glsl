#include "./shading_context.glsl"
#include "../material/material_context.glsl"

ShadingContext computeShadingContext(const MaterialContext mctx) {
    vec4 color = computeMaterialColor(mctx);
    vec3 normal = computeMaterialNormal(mctx);
    float ao = computeMaterialAO(mctx);

    return computeShadingContext(
        color,
        mctx.metallic,
        getVertexPosition(), //TODO: make an argument?
        mctx.roughness,
        normal,
        ao
    );
}