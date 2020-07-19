GraphicsEngine::GLSL_VERSION

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;

out vec2 vs_uv;

void main()
{
    vs_uv = uv;
	gl_Position = vec4(pos, 1.0);
}