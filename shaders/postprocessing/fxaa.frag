ENGINE::COMMON

in vec2 uv;

out vec3 color;

uniform sampler2D scene;
uniform float fxaa_subpix;
uniform float fxaa_edge_threshold;
uniform float fxaa_edge_threshold_min;

#define FXAA_PC 1
#define FXAA_GLSL_130 1
#ifdef ENGINE_SETTINGS_FXAA_QUALITY_PRESET
#define FXAA_QUALITY__PRESET ENGINE_SETTINGS_FXAA_QUALITY_PRESET
#else
#define FXAA_QUALITY__PRESET 29
#endif
#define FXAA_GREEN_AS_LUMA 1
#define FXAA_GATHER4_ALPHA 0

#include "../functions/fxaa.glsl"

#include "../pipeline/scene.glsl"

void main() {
    color = FxaaPixelShader(
        uv,
        vec4(0),
        scene,
        scene,
        scene,
        1.0 / getResolution(),
        vec4(0),
        vec4(0),
        vec4(0),
        fxaa_subpix,
        fxaa_edge_threshold,
        fxaa_edge_threshold_min,
        0,
        0,
        0,
        vec4(0)
    ).rgb;
}