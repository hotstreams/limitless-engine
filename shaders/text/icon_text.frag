ENGINE::COMMON

in vec2 vs_uv;
in vec4 vs_color;

out vec4 FragColor;

uniform sampler2D bitmap;

void main() {
    FragColor = vs_color * texture(bitmap, vs_uv);
}