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

#include "glsl/material.glsl"

#include "glsl/scene.glsl"

void main()
{
    vec2 uv = in_data.uv;

    #include "glsl/material_variables.glsl"

    Limitless::CustomMaterialFragmentCode

    #if defined(MATERIAL_BLENDMASK)
        if (mat_blend_mask <= 0.5) discard;
        //fragment_color.a *= mat_blend_mask;
    #endif
}