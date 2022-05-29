Limitless::GLSL_VERSION

#extension GL_ARB_tessellation_shader : enable
Limitless::Extensions

Limitless::Settings
Limitless::MaterialType
Limitless::ModelType
Limitless::EmitterType

#include "../pipeline/material/material.glsl"

layout (vertices = 4) out;

#include "../pipeline/interface_block/tess_control_input.glsl"
#include "../pipeline/interface_block/tess_control_output.glsl"
#include "../pipeline/interface_block/tess_control_pass.glsl"

void main() {
    InterfaceBlockPassThrough();

    vec2 factor = getMaterialTesselationFactor();
    gl_TessLevelOuter[0] = factor.x;
    gl_TessLevelOuter[1] = factor.x;
    gl_TessLevelOuter[2] = factor.x;
    gl_TessLevelOuter[3] = factor.x;
    gl_TessLevelInner[0] = factor.y;
    gl_TessLevelInner[1] = factor.y;
}