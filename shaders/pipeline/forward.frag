ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

#include "../interface_block/fragment.glsl"
#include "../shading/shading_mctx.glsl"

out vec4 color;

void main() {
    MaterialContext mctx = computeMaterialContext();
    color = shadeFragment(mctx);
}