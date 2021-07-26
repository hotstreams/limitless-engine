Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings

#include "../../functions/tone_mapping.glsl"

in vec2 uv;

out vec3 color;

uniform sampler2D lightened;
uniform sampler2D emissive;

void main() {
    color = texture(lightened, uv).rgb + toneMapping(texture(emissive, uv).rgb, 1.0);
}