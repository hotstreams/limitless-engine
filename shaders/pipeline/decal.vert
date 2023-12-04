ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

#include "../interface_block/vertex.glsl"
#include "./scene.glsl"
#include "../instance/instance.glsl"
#include "../material/material.glsl"
#include "../interface_block/pass_through.glsl"

void main() {
    #if !defined (SpriteEmitter)
       vec2 uv = getVertexUV();
    #else
       vec2 uv = vec2(0.0);
    #endif

    vec3 vertex_position = getVertexPosition();

    ENGINE_MATERIAL_VERTEX_SNIPPET

    mat4 model_transform = getModelTransform();

    vec4 world_position = model_transform * vec4(vertex_position, 1.0);

    gl_Position = getViewProjection() * world_position;

    InterfaceBlockPassThrough(world_position.xyz, uv, model_transform);
}
