ENGINE::COMMON

#include "../functions/tone_mapping.glsl"

in vec2 uv;

out vec3 color;

uniform sampler2D lightened;

uniform sampler2D bloom;
uniform sampler2D outline;
uniform float bloom_strength;
uniform float tone_mapping_exposure;

void main() {
    vec3 bloom_color = texture(bloom, uv).rgb * bloom_strength;
    color = texture(lightened, uv).rgb + bloom_color;

    // apply tone mapping function to HDR
    color = toneMapping(color, tone_mapping_exposure);

    // apply gamma correction
    float gamma = 2.2;
    color = pow(color, vec3(1.0 / gamma));

    // add objects outlining
    color += texture(outline, uv).rgb;
}