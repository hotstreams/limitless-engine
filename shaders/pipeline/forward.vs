GraphicsEngine::GLSL_VERSION
GraphicsEngine::Extensions

GraphicsEngine::MaterialType

#include "glsl/output_data_vs.glsl"
#include "glsl/material.glsl"

layout(location = 0) in vec3 position;
#ifdef MATERIAL_LIT
    layout(location = 1) in vec3 normal;
    #ifdef NORMAL_MAPPING
        layout(location = 2) in vec3 tangent;
    #endif
#endif
layout(location = 3) in vec2 uv;

uniform mat4 MVP;

void main()
{
    #ifdef MATERIAL_LIT
        fs_data.world_position = MVP * position;
    #endif

    fs_data.uv = uv;

	gl_Position = MVP * position;
}