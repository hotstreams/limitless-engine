GraphicsEngine::GLSL_VERSION

out vec4 color;

in vec2 vs_uv;

uniform sampler2D diffuse;

void main()
{
	color = texture(diffuse, vs_uv);
}