ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

ENGINE::VERTEX_STREAM
ENGINE::VERTEX_CONTEXT
ENGINE::INTERFACE_BLOCK_OUT

#include "../pipeline/scene.glsl"
#include "../instance/instance.glsl"
#include "../material/material.glsl"
#include "../vertex/vertex_context.glsl"

void main() {
    VertexContext vctx = computeVertexContext();
    InstanceContext ictx = computeInstanceContext(vctx);
    EvalContext ectx = computeEvalContext(vctx, ictx);

    CustomVertexContext(vctx, ictx, ectx);

    ProcessEvalContext(vctx, ictx, ectx);

    VertexPassThrough(vctx, ictx, ectx);
}
