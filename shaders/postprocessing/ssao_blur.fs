Limitless::GLSL_VERSION
Limitless::Extensions

#include "../pipeline/scene.glsl"

in vec2 uv;

uniform sampler2D ssao;

out float color;

void main() {
    vec2 texel_size = 1.0 / getResolution();

    float result = 0.0;
    for (int x = -2; x < 2; ++x) {
        for (int y = -2; y < 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texel_size;
            result += texture(ssao, uv + offset).r;
        }
    }

    color = result / (4.0 * 4.0);
}