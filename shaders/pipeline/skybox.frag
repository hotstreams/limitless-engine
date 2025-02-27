ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

ENGINE::INTERFACE_BLOCK_IN
ENGINE::FRAGMENT_CONTEXT

#include "./scene.glsl"
#include "../instance/instance.glsl"
#include "../material/material_context.glsl"
#include "../shading/shading_mctx.glsl"

layout (location = 0) out vec3 albedo;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 properties;
layout (location = 3) out vec3 emissive;
layout (location = 4) out vec3 info;
layout (location = 5) out vec4 outline;

void main() {
    VertexContext vctx = computeVertexContext();
    InstanceContext ictx = computeInstanceContext(vctx);
    MaterialContext mctx = computeMaterialContext(vctx);

    albedo = computeMaterialColor(mctx).rgb;

    normal = computeMaterialNormal(mctx);

    properties.r = mctx.roughness;
    properties.g = mctx.metallic;
    properties.b = computeMaterialAO(mctx);

    info.r = float(mctx.shading_model) / 255.0;
    info.g = 0.0;
    info.b = 0.0;

    outline = vec4(0.0);

    emissive = computeMaterialEmissiveColor(mctx);
}
