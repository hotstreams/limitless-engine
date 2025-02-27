ENGINE::COMMON

out vec3 color;

in vec2 uv;

uniform sampler2D image;
uniform float threshold;

void main() {
    vec3 source = texture(image, uv).rgb;
    color = max(vec3(0.0), source - vec3(threshold));
}