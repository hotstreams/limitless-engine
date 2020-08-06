GraphicsEngine::GLSL_VERSION
GraphicsEngine::Extensions

#include "glsl/scene.glsl"

layout (location = 0) in vec3 position;

out vec3 uv;

void main()
{
    uv = position;

    vec4 pos = projection * mat4(mat3(view)) * vec4(position, 1.0);
    gl_Position = pos.xyww;
}