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

    vec4 color = getGBufferColor(uv);
    gctx.color = color.rgb;
    gctx.position = getGBufferPosition(uv);
    gctx.normal = getGBufferNormal(uv);
    gctx.emissive_color = getGBufferEmissiveColor(uv);
    gctx.ao = color.a;

    vec3 props = getGBufferProps(uv);

    gctx.roughness = props.r;
    gctx.metallic = props.g;
    gctx.shading_model = uint(props.b * 255.0);

    return gctx;
}