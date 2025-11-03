ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

ENGINE::INTERFACE_BLOCK_IN
ENGINE::FRAGMENT_CONTEXT

#include "./scene.glsl"
#include "../instance/instance.glsl"
#include "../material/material_context.glsl"
#include "../shading/shading_mctx.glsl"

out vec3 color;

vec3 getColorId(uint id) {
    uint r = (id & 0x000000FFu) >> 0;
    uint g = (id & 0x0000FF00u) >> 8;
    uint b = (id & 0x00FF0000u) >> 16;
    return vec3(r, g, b) / 255.0;
}

void main() {
    VertexContext vctx = computeVertexContext();
    InstanceContext ictx = computeInstanceContext(vctx);
    MaterialContext mctx = computeMaterialContext(vctx);

    shadeFragment(mctx);

    color = getColorId(ictx.id);
}



