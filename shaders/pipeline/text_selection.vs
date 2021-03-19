LimitlessEngine::GLSL_VERSION
LimitlessEngine::Extensions

layout (location = 0) in vec2 position;

uniform mat4 model;

uniform mat4 proj;

void main()
{
    gl_Position = proj * model * vec4(position, 0.0, 1.0);
}