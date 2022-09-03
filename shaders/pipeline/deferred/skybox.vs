Limitless::GLSL_VERSION
Limitless::Extensions

#include "../interface_block/vertex.glsl"
#include "../scene.glsl"

layout (location = 0) in vec3 vertex_position;

out vec3 uv;

void main() {
    uv = vertex_position;

    vec4 pos = getProjection() * mat4(mat3(getView())) * vec4(vertex_position, 1.0);
    gl_Position = pos.xyww;
}
