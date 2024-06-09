ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

#include "../interface_block/fragment.glsl"
#include "../shading/shading_mctx.glsl"

void main() {
    MaterialContext mctx = computeMaterialContext();
    shadeFragment(mctx);
}
