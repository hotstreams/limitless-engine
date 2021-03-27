LimitlessEngine::GLSL_VERSION
LimitlessEngine::Extensions

#include "glsl/scene_lighting.glsl"

out vec4 color;

in vec3 uv;

uniform samplerCube skybox;

void main() {
    color = texture(skybox, uv) * vec4(ambient_color.rgb, 1.0) * ambient_color.a;
}