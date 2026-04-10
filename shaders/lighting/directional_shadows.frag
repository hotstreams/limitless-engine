ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

ENGINE::INTERFACE_BLOCK_IN
ENGINE::FRAGMENT_CONTEXT

#include "../pipeline/scene.glsl"
#include "../instance/instance.glsl"
#include "../material/material_context.glsl"
#include "../functions/lod_crossfade.glsl"

void main() {
    VertexContext vctx = computeVertexContext();
    InstanceContext ictx = computeInstanceContext(vctx);
    MaterialContext mctx = computeMaterialContext(vctx);

    // Directional shadow-map pass is depth-only.
    // We still evaluate material inputs to support alpha-test / masked materials (discard).
    // IMPORTANT: Do NOT call full lighting / shading here, otherwise we end up sampling shadow maps
    // while generating them (undefined behavior and wrong results).
    vec4 _unused = computeMaterialColor(mctx);
    applyEngineLodCrossFade(ictx);
    // Mark as used without affecting output (depth-only pass).
    if (_unused.a < -1.0) {
        discard;
    }
}
