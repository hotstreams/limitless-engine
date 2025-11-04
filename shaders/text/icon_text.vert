ENGINE::COMMON

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 color;

out vec2 vs_uv;
out vec4 vs_color;

uniform mat4 model;

uniform mat4 proj;

void main()
{
    gl_Position = proj * model * vec4(position, 0.0, 1.0);
    vs_uv = uv;
    vs_color = color;
}
