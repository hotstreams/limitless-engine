GraphicsEngine::GLSL_VERSION

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;

out vec2 vs_uv;

uniform mat4 VP;

void main()
{
    vs_uv = uv;
	gl_Position = VP * vec4(pos, 1.0);
}