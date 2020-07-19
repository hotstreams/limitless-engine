GraphicsEngine::GLSL_VERSION

out vec4 color;

in vec2 vs_uv;

uniform vec4 triangle_color;
uniform sampler2D diffuse;

void main()
{
	color = texture(diffuse, vs_uv) * triangle_color;
}