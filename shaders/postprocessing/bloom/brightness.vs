Limitless::GLSL_VERSION
Limitless::Extensions

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

out vec2 fs_uv;

void main() {
    fs_uv = uv;
    gl_Position = vec4(position, 1.0);
}