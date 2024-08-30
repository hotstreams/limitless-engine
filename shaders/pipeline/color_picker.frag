ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

#include "../interface_block/fragment.glsl"
#include "../shading/shading_mctx.glsl"
#include "../instance/instance_fs.glsl"

out vec3 color;

vec3 getColorId() {
    uint id = getId();
    uint r = (id & 0x000000FFu) >> 0;
    uint g = (id & 0x0000FF00u) >> 8;
    uint b = (id & 0x00FF0000u) >> 16;
    return vec3(r, g, b) / 255.0;
}

void main() {
    MaterialContext mctx = computeMaterialContext();
    shadeFragment(mctx);

    color = getColorId();
}



