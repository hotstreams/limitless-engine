ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

ENGINE::VERTEX_STREAM
ENGINE::VERTEX_CONTEXT
ENGINE::VERTEX_CONTEXT_PASS_THROUGH

#include "../pipeline/scene.glsl"
#include "../instance/instance.glsl"
#include "../material/material.glsl"


generate vertex stream
generate vertex stream getters

generate vertex context
generate vertex context compute

generate vertex context out interface block

VertexContext computeVertexContext() {
    VertexContext vctx;

    ENGINE::VERTEX_CONTEXT_ASSIGN_STREAM_ATTIRIBUTES

    vctx.model_transform = getModelTransform();
}

void main() {
    VertexContext vctx = computeVertexContext();
    CustomVertexContext(vctx);

    vctx.world_position = vctx.model_transform * vec4(vctx.vertex_position, 1.0);
    gl_Position = getViewProjection() * world_position;

    VertexContextPassThrough(vctx);


    #if !defined (SpriteEmitter)
       vec2 uv = getVertexUV();
    #else
       vec2 uv = vec2(0.0);
    #endif

    vec3 vertex_position = getVertexPosition();

    ENGINE_MATERIAL_VERTEX_SNIPPET

    mat4 model_transform = getModelTransform();

    vec4 world_position = model_transform * vec4(vertex_position, 1.0);

    #if !defined (MATERIAL_TESSELATION_FACTOR)
        gl_Position = getViewProjection() * world_position;
    #endif

    InterfaceBlockPassThrough(world_position.xyz, uv, model_transform);
}
