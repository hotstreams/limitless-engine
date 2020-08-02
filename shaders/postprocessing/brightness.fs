GraphicsEngine::GLSL_VERSION
GraphicsEngine::Extensions

out vec4 color;

in vec2 fs_uv;

uniform sampler2D image;

void main()
{
    vec3 source_color = texture(image, fs_uv).rgb;

    float brightness = dot(source_color, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0) {
        color = vec4(source_color, 1.0);
    } else {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}