Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::ModelType
Limitless::EmitterType

#include "../interface_block/fragment.glsl"
#include "../scene.glsl"
#include "../material/material.glsl"
#include "../material/fragment_properties.glsl"

// 0: RGB - albedo; A - ao;
// 1: RGB - normal; A - shading model;
// 2: R - metallic; G - roughness;
// 3: RGB - emissive

layout (location = 0) out vec4 g_albedo;
layout (location = 1) out vec4 g_normal;
layout (location = 2) out vec2 g_props;
layout (location = 3) out vec3 g_emissive;

void main() {
    vec4 albedo = getFragmentAlbedo();
    uint shading_model = getMaterialShadingModel();
    vec3 normal = getFragmentNormal();
    float ao = getFragmentAO();
    float metallic = getFragmentMetallic();
    float roughness = getFragmentRoughness();
    vec3 emissive = getFragmentEmissive();

    #if defined (MATERIAL_REFRACTION)
        float refraction = getMaterialRefraction();
    #endif

    /*               */

    // do whatever you want
    //
    // %albedo
    // %shading_model
    // %normal
    // %ao
    // %metallic
    // %roughness
    // %emissive

    _MATERIAL_FRAGMENT_SNIPPET

    /*               */

    g_albedo.rgb = albedo.rgb;
    g_albedo.a = ao;

    g_normal.rgb = normal;
    g_normal.a = shading_model;

    g_props.r = metallic;
    g_props.g = roughness;

    g_emissive = emissive;
}
