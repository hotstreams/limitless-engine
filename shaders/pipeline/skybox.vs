Limitless::GLSL_VERSION
Limitless::Extensions

#include "glsl/scene.glsl"

layout (location = 0) in vec3 position;

out vec3 fs_uv;

void main() {
    fs_uv = position;

    vec4 pos = projection * mat4(mat3(view)) * vec4(position, 1.0);
    gl_Position = pos.xyww;
}