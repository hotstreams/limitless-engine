ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

#include "../interface_block/fragment.glsl"
#include "../shading/shading_mctx.glsl"

out vec3 color;

uniform vec3 color_id;

void main() {
    MaterialContext mctx = computeMaterialContext();
    shadeFragment(mctx);

    color = color_id;
}



