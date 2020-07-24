GraphicsEngine::GLSL_VERSION
GraphicsEngine::Extensions
GraphicsEngine::Settings
GraphicsEngine::MaterialType
GraphicsEngine::ModelType

#include "glsl/input_data_fs.glsl"
#include "glsl/material.glsl"

#include "glsl/scene.glsl"

#ifdef MATERIAL_LIT
    #include "glsl/light_computation.glsl"
#endif

out vec4 color;

void main()
{
    #include "glsl/material_variables.glsl"

    #ifdef CUSTOM_MATERIAL
        GraphicsEngine::CustomMaterialFragmentCode
    #endif

    // computing final color
    vec4 fragment_color = vec4(1.0);
    #include "glsl/material_computation.glsl"

	color = fragment_color;
}