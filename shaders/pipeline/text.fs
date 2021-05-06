Limitless::GLSL_VERSION
Limitless::Extensions

in vec2 vs_uv;

out vec4 FragColor;

uniform sampler2D bitmap;
uniform vec4 color;

void main()
{
    FragColor = vec4(color.rgb, texture(bitmap, vs_uv).r * color.a);
}