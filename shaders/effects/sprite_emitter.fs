Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::EmitterType

#include "glsl/material.glsl"
#include "glsl/scene.glsl"

#if defined(InitialColor_MODULE)
    in vec4 fs_color;
#endif
#if defined(InitialRotation_MODULE)
    in vec3 fs_rotation;
#endif
#if defined(InitialVelocity_MODULE)
    in vec3 fs_velocity;
#endif
#if defined(Lifetime_MODULE)
    in float fs_lifetime;
#endif
#if defined(InitialSize_MODULE)
    in vec3 fs_size;
#endif
#if defined(SubUV_MODULE)
    in vec4 fs_subUV;
#endif
#if defined(CustomMaterial_MODULE)
    in vec4 fs_properties;
#endif

out vec4 color;

void main()
{
    vec2 uv = gl_PointCoord;

    #if defined(InitialRotation_MODULE)
        vec2 center = vec2(0.5, 0.5);
        vec2 translated = uv - center;
        mat2 rotation = mat2(cos(fs_rotation.x), sin(fs_rotation.x), -sin(fs_rotation.x), cos(fs_rotation.x));
        translated = rotation * translated;
        translated = translated + center;
        uv = clamp(translated, 0.0, 1.0);
    #endif

    #if defined(SubUV_MODULE)
        uv = uv * p_subUV.xy + p_subUV.zw;
    #endif

    #include "glsl/material_variables.glsl"

    Limitless::CustomMaterialFragmentCode

    // computing final color
    vec4 fragment_color = vec4(1.0);
    #if defined(InitialColor_MODULE)
        fragment_color *= fs_color;
    #endif

    #if defined(MATERIAL_COLOR)
        fragment_color *= mat_color;
    #endif

    #if defined(MATERIAL_DIFFUSE)
        fragment_color *= mat_diffuse;
    #endif

    #if defined(MATERIAL_BLENDMASK)
        if (mat_blend_mask <= 0.5) discard;
        //fragment_color.a *= mat_blend_mask;
    #endif

    #if defined(MATERIAL_EMISSIVEMASK)
        if (mat_emissivemask != vec3(0.0)) {
            fragment_color.rgb *= mat_emissive_mask;

            #if defined(MATERIAL_EMISSIVE_COLOR)
                fragment_color.rgb *= mat_emissive_color;
            #endif
        }
    #else
        #if defined(MATERIAL_EMISSIVE_COLOR)
            fragment_color.rgb *= mat_emissive_color;
        #endif
    #endif

    color = fragment_color;
}
