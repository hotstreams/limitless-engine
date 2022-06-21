Limitless::GLSL_VERSION
Limitless::Extensions

in vec2 uv;

out vec3 color;

uniform sampler2D source;
uniform float level;
uniform vec4 resolution;

void main() {
    const float radius = 1.0;
    vec4 d = vec4(resolution.zw, -resolution.zw) * radius;
    vec3 c0, c1;
    c0  = textureLod(source, uv + d.zw, level).rgb;
    c0 += textureLod(source, uv + d.xw, level).rgb;
    c0 += textureLod(source, uv + d.xy, level).rgb;
    c0 += textureLod(source, uv + d.zy, level).rgb;
    c0 += 4.0 * textureLod(source, uv, level).rgb;
    c1  = textureLod(source, uv + vec2(d.z,  0.0), level).rgb;
    c1 += textureLod(source, uv + vec2(0.0,  d.w), level).rgb;
    c1 += textureLod(source, uv + vec2(d.x,  0.0), level).rgb;
    c1 += textureLod(source, uv + vec2( 0.0, d.y), level).rgb;
    color = (c0 + 2.0 * c1) * (1.0 / 16.0);
}
