LimitlessEngine::GLSL_VERSION
LimitlessEngine::Extensions
LimitlessEngine::Settings
LimitlessEngine::MaterialType
LimitlessEngine::ModelType

#include "glsl/scene.glsl"
#include "glsl/input_data_fs.glsl"
#include "glsl/material.glsl"

#ifdef MATERIAL_LIT
    #include "glsl/scene_lighting.glsl"
    #include "glsl/light_computation.glsl"
#endif

out vec4 color;

void main()
{
    vec2 uv = fs_data.uv;
    #include "glsl/material_variables.glsl"

    #ifdef CUSTOM_MATERIAL
        LimitlessEngine::CustomMaterialFragmentCode
    #endif

    // computing final color
    vec4 fragment_color = vec4(1.0);
    #include "glsl/material_computation.glsl"

	color = fragment_color;
}