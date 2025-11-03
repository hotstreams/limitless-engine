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

    vec4 pos = getProjection() * mat4(mat3(getView())) * vec4(vctx.position, 1.0);
    gl_Position = pos.xyww;

    VertexPassThrough(vctx, ictx, ectx);
}