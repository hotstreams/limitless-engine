ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

#include "../instance/instance.glsl"
#include "../material/material.glsl"
#include "../vertex/vertex_context.glsl"
#include "../pipeline/scene.glsl"

void main() {
    VertexContext vctx = computeVertexContext();
    CustomVertexContext(vctx);

    vctx.world_position = vctx.model_transform * vec4(vctx.position, 1.0);
    gl_Position = getViewProjection() * vctx.world_position;

    VertexContextPassThrough(vctx);
}
