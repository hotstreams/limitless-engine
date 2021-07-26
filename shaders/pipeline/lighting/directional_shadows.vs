Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::ModelType
Limitless::EmitterType

#include "../vertex_streams/vertex_stream.glsl"
#include "../interface_block/vertex.glsl"

#include "../scene.glsl"
#include "../instance/instance.glsl"
#include "../material/material.glsl"
#include "../interface_block/pass_through.glsl"

uniform mat4 light_space;

void main() {
    #if !defined (SpriteEmitter)
       vec2 uv = getVertexUV();
    #else
       vec2 uv = vec2(0.0);
    #endif

    vec3 vertex_position = getVertexPosition();

    // %uv
    // %vertex_position
    _MATERIAL_VERTEX_SNIPPET

    mat4 model_transform = getModelTransform();

    vec4 world_position = model_transform * vec4(vertex_position, 1.0);

    #if !defined (MATERIAL_TESSELATION_FACTOR)
        gl_Position = light_space * world_position;
    #endif

    InterfaceBlockPassThrough(world_position.xyz, uv, model_transform);
}
