ENGINE::COMMON

in vec2 uv;

out vec3 color;

uniform sampler2D scene;

#define FXAA_PC 1
#define FXAA_GLSL_130 1
#define FXAA_QUALITY__PRESET 29
#define FXAA_GREEN_AS_LUMA 1
#define FXAA_GATHER4_ALPHA 0

#include "../functions/fxaa.glsl"

const float fxaaSubpix = 0.75;
const float fxaaEdgeThreshold = 0.166;
const float fxaaEdgeThresholdMin = 0.0833;

#include "../pipeline/scene.glsl"

void main() {
    color = FxaaPixelShader(
        uv - 1.0 / getResolution(),
        vec4(0),
        scene,
        scene,
        scene,
        1.0 / getResolution(),
        vec4(0),
        vec4(0),
        vec4(0),
        fxaaSubpix,
        fxaaEdgeThreshold,
        fxaaEdgeThresholdMin,
        0,
        0,
        0,
        vec4(0)
    ).rgb;
}