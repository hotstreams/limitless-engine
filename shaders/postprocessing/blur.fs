LimitlessEngine::GLSL_VERSION
LimitlessEngine::Extensions

in vec2 fs_uv;

out vec4 color;

uniform vec2 direction;
uniform sampler2D image;

void main()
{
    vec4 c = vec4(0.0);
    vec2 off1 = vec2(1.3846153846) * direction;
    vec2 off2 = vec2(3.2307692308) * direction;
    vec2 resolution = 1.0 / textureSize(image, 0);

    c += texture2D(image, fs_uv) * 0.2270270270;
    c += texture2D(image, fs_uv + off1 * resolution) * 0.3162162162;
    c += texture2D(image, fs_uv - off1 * resolution) * 0.3162162162;
    c += texture2D(image, fs_uv + off2 * resolution) * 0.0702702703;
    c += texture2D(image, fs_uv - off2 * resolution) * 0.0702702703;
    color = c;
}