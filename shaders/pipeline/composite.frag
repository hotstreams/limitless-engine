ENGINE::COMMON

#include "../functions/tone_mapping.glsl"

in vec2 uv;

out vec3 color;

uniform sampler2D lightened;

uniform sampler2D bloom;
uniform sampler2D outline;
uniform float outline_strength;
uniform float bloom_strength;
uniform float tone_mapping_exposure;

void main() {
    //if (bloom_strength != 0.0) {
    //    vec3 bloom_color = texture(bloom, uv).rgb * bloom_strength;
    //    color = texture(lightened, uv).rgb + bloom_color;
    //}

    color = texture(lightened, uv).rgb;
    // apply tone mapping function to HDR
    color = toneMapping(color, tone_mapping_exposure);

    // apply gamma correction
    float gamma = 2.2;
    color = pow(color, vec3(1.0 / gamma));

    // add objects outlining
    //if (outline_strength != 0.0) {
    //    color += texture(outline, uv).rgb * outline_strength;
    //}
}