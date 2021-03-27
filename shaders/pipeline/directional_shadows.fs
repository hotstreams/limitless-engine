LimitlessEngine::GLSL_VERSION
LimitlessEngine::Extensions
LimitlessEngine::Settings
LimitlessEngine::MaterialType
LimitlessEngine::ModelType

#include "glsl/input_data_fs.glsl"
#include "glsl/material.glsl"

#include "glsl/scene.glsl"

#ifdef MATERIAL_LIT
    #include "glsl/light_computation.glsl"
#endif

void main()
{
    vec2 uv = vs_in.frag_uv;

    #include "glsl/material_values.glsl"

    ENGINE::DYNAMIC_MATERIAL_FRAGMENT_ACTIONS

    #ifdef _MATERIAL_BLEND_MASK_
       if(mat_blend_mask == 0.0) discard;
    #endif
}

void main()
{
    vec2 uv = fs_data.uv;

    #include "glsl/material_variables.glsl"

    #ifdef CUSTOM_MATERIAL
        LimitlessEngine::CustomMaterialFragmentCode
    #endif

    #ifdef MATERIAL_BLENDMASK
        if (mat_blend_mask <= 0.5) discard;
        //fragment_color.a *= mat_blend_mask;
    #endif
}