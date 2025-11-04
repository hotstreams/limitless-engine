ENGINE::COMMON

#include "../functions/tone_mapping.glsl"

in vec2 uv;

out vec3 color;

uniform sampler2D lightened;

uniform sampler2D outline;
uniform float tone_mapping_exposure;
uniform float gamma;

void main() {
    color = texture(lightened, uv).rgb;

    // apply tone mapping function to HDR
    color = toneMapping(color, tone_mapping_exposure);

    // apply gamma correction
    color = pow(color, vec3(1.0 / gamma));

    // add objects outlining
    //color += texture(outline, uv).rgb;
}