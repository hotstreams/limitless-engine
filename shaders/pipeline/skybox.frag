ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

in vec3 skybox_uv;

#include "../interface_block/fragment.glsl"
#include "../shading/shading_mctx.glsl"

layout (location = 0) out vec3 albedo;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 properties;
layout (location = 3) out vec3 emissive;
layout (location = 4) out vec3 info;

void main() {
    const MaterialContext mctx = computeMaterialContext();

    albedo = computeMaterialColor(mctx).rgb;

    normal = computeMaterialNormal(mctx);

    properties.r = mctx.roughness;
    properties.g = mctx.metallic;
    properties.b = computeMaterialAO(mctx);

    info.r = float(mctx.shading_model) / 255.0;

    emissive = computeMaterialEmissiveColor(mctx);
}
