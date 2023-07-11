Limitless::GLSL_VERSION
Limitless::Extensions

in vec2 uv;

out vec3 color;

uniform sampler2D screen_texture;

void main() {
    color = texture(screen_texture, uv).rgb;
}