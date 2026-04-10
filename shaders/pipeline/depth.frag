ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

ENGINE::INTERFACE_BLOCK_IN
ENGINE::FRAGMENT_CONTEXT

#include "./scene.glsl"
#include "../instance/instance.glsl"
#include "../material/material_context.glsl"
#include "../functions/lod_crossfade.glsl"
#include "../shading/shading_mctx.glsl"

void main() {
    VertexContext vctx = computeVertexContext();
    InstanceContext ictx = computeInstanceContext(vctx);
    MaterialContext mctx = computeMaterialContext(vctx);
    vec4 _lod_surface = computeMaterialColor(mctx);
    applyEngineLodCrossFade(ictx);
    shadeFragment(mctx);
}
