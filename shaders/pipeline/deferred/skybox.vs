Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::ModelType

#include "../vertex_streams/vertex_stream.glsl"
#include "../interface_block/vertex.glsl"
#include "../shading/common.glsl"
#include "../scene.glsl"
#include "../instance/instance.glsl"
#include "../material/material.glsl"
#include "../interface_block/pass_through.glsl"

out vec3 skybox_uv;

void main() {
    vec2 uv = vec2(0.0);

    vec3 vertex_position = getVertexPosition();

    // %uv
    // %vertex_position
    _MATERIAL_VERTEX_SNIPPET

    skybox_uv = getVertexPosition();

    vec4 pos = getProjection() * mat4(mat3(getView())) * vec4(vertex_position, 1.0);
    gl_Position = pos.xyww;

    InterfaceBlockPassThrough(pos.xyw, uv, getModelTransform());
}