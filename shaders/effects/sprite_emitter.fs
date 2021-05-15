Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::EmitterType

#include "../glsl/material.glsl"
#include "../glsl/scene.glsl"

in vertex_data {
    #if defined(InitialColor_MODULE)
        vec4 color;
    #endif
    #if defined(InitialRotation_MODULE)
        vec3 rotation;
    #endif
    #if defined(InitialVelocity_MODULE)
        vec3 velocity;
    #endif
    #if defined(Lifetime_MODULE)
        float lifetime;
    #endif
    #if defined(InitialSize_MODULE)
        vec3 size;
    #endif
    #if defined(SubUV_MODULE)
        vec4 subUV;
    #endif
    #if defined(CustomMaterial_MODULE)
        vec4 properties;
    #endif
} in_data;

out vec4 color;

void main() {
    vec2 uv = gl_PointCoord;

    #if defined(InitialRotation_MODULE)
        vec2 center = vec2(0.5, 0.5);
        vec2 translated = uv - center;
        mat2 rotation = mat2(cos(in_data.rotation.x), sin(in_data.rotation.x), -sin(in_data.rotation.x), cos(in_data.rotation.x));
        translated = rotation * translated;
        translated = translated + center;
        uv = clamp(translated, 0.0, 1.0);
    #endif

    #if defined(SubUV_MODULE)
        uv = uv * in_data.subUV.xy + in_data.subUV.zw;
    #endif

    #include "../glsl/material_variables.glsl"

    Limitless::CustomMaterialFragmentCode

    // computing final color
    vec4 fragment_color = vec4(1.0);
    #if defined(InitialColor_MODULE)
        fragment_color *= in_data.color;
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
