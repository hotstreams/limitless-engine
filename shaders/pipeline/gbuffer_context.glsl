#include "./gbuffer.glsl"

struct GBufferContext {
    vec3 color;
    vec3 position;
    vec3 normal;
    vec3 emissive_color;
    float roughness;
    float metallic;
    float ao;
    uint shading_model;
};

GBufferContext computeGBufferContext(vec2 uv) {
    GBufferContext gctx;

    gctx.color = getGBufferColor(uv);
    gctx.position = getGBufferPosition(uv);
    gctx.normal = getGBufferNormal(uv);
    gctx.emissive_color = getGBufferEmissiveColor(uv);

    vec3 props = getGBufferProps(uv);

    gctx.roughness = props.r;
    gctx.metallic = props.g;
    gctx.ao = props.b;

    gctx.shading_model = uint(getGBufferInfo(uv).r * 255.0);

    return gctx;
}