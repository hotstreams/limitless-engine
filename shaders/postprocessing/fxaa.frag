Limitless::GLSL_VERSION
Limitless::Extensions

in vec2 uv;

out vec3 color;

uniform sampler2D scene;

#include "../functions/fxaa.glsl"

void main() {
    color = fxaaProcess(scene, uv);
}