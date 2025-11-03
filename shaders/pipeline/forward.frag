ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

ENGINE::INTERFACE_BLOCK_IN
ENGINE::FRAGMENT_CONTEXT

#include "./scene.glsl"
#include "../instance/instance.glsl"
#include "../material/material_context.glsl"
#include "../shading/shading_mctx.glsl"

out vec4 color;

void main() {
    VertexContext vctx = computeVertexContext();
    InstanceContext ictx = computeInstanceContext(vctx);
    MaterialContext mctx = computeMaterialContext(vctx);

    color = shadeFragment(mctx);
}