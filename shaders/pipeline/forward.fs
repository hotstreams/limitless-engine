Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::ModelType

in vertex_data {
    #if defined(MATERIAL_LIT) || defined(MATERIAL_TESSELLATION_FACTOR)
        vec3 world_position;
    #endif

    #if defined(MATERIAL_LIT)
        #if defined(MATERIAL_NORMAL) && defined(NORMAL_MAPPING)
            mat3 TBN;
        #else
            vec3 normal;
        #endif
    #endif

    vec2 uv;
} in_data;

#include "glsl/scene.glsl"
#include "glsl/material.glsl"

#if defined(MATERIAL_LIT)
    #include "glsl/scene_lighting.glsl"
    #include "glsl/light_computation.glsl"
#endif

out vec4 color;

void main()
{
    vec2 uv = in_data.uv;
    #include "glsl/material_variables.glsl"

    Limitless::CustomMaterialFragmentCode

    // computing final color
    vec4 fragment_color = vec4(1.0);
    #include "glsl/material_computation.glsl"

	color = fragment_color;
}