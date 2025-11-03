ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

ENGINE::INTERFACE_BLOCK_IN
ENGINE::FRAGMENT_CONTEXT

#include "../pipeline/scene.glsl"
#include "../instance/instance.glsl"
#include "../material/material_context.glsl"
#include "../shading/shading_mctx.glsl"

void main() {
    VertexContext vctx = computeVertexContext();
    MaterialContext mctx = computeMaterialContext(vctx);
    shadeFragment(mctx);
}
