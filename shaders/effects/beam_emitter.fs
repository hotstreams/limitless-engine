Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::EmitterType

#include "glsl/material.glsl"
#include "glsl/scene.glsl"

in vec2 fs_uv;

#ifdef InitialColor_MODULE
    in vec4 fs_color;
#endif
#ifdef InitialRotation_MODULE
    in vec3 fs_rotation;
#endif
#ifdef InitialVelocity_MODULE
    in vec3 fs_velocity;
#endif
#ifdef Lifetime_MODULE
    in float fs_lifetime;
#endif
#ifdef InitialSize_MODULE
    in float fs_size;
#endif
#ifdef SubUV_MODULE
    in vec4 fs_subUV;
#endif
#ifdef CustomMaterial_MODULE
    in vec4 fs_properties;
#endif

out vec4 color;

void main() {
    vec2 uv = fs_uv;

    #ifdef SubUV_MODULE
        uv = uv * p_subUV.xy + p_subUV.zw;
    #endif

    #include "glsl/material_variables.glsl"

    Limitless::CustomMaterialFragmentCode

    // computing final color
    vec4 fragment_color = vec4(1.0);
    #ifdef InitialColor_MODULE
        fragment_color *= fs_color;
    #endif

    #ifdef MATERIAL_COLOR
        fragment_color *= mat_color;
    #endif

    #ifdef MATERIAL_DIFFUSE
        fragment_color *= mat_diffuse;
    #endif

    #ifdef MATERIAL_BLENDMASK
        if (mat_blend_mask == 0.0) discard;
        fragment_color.a *= mat_blend_mask;
    #endif

    #ifdef MATERIAL_EMISSIVEMASK
        if (mat_emissivemask != vec3(0.0))
            {
                fragment_color.rgb *= mat_emissive_mask;

                #ifdef MATERIAL_EMISSIVE_COLOR
                    fragment_color.rgb *= mat_emissive_color;
                #endif
            }
    #else
        #ifdef MATERIAL_EMISSIVE_COLOR
            fragment_color.rgb *= mat_emissive_color;
        #endif
    #endif

    color = fragment_color;
}