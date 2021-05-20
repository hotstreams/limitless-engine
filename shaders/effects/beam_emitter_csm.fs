Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::EmitterType

#include "../glsl/material.glsl"
#include "../glsl/scene.glsl"

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

void main() {
    vec2 uv = fs_uv;

    #ifdef SubUV_MODULE
        uv = uv * fs_subUV.xy + fs_subUV.zw;
    #endif

    #include "../glsl/material_variables.glsl"

    Limitless::CustomMaterialFragmentCode

    #ifdef MATERIAL_BLENDMASK
        if (mat_blend_mask == 0.0) discard;
    #endif
}