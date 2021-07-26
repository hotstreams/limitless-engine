Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::ModelType

vec3 getVertexNormal() {
    return vec3(0.0);
}

#include "../scene.glsl"
#include "../material/material.glsl"
#include "../material/fragment_properties.glsl"

layout (location = 0) out vec4 g_albedo;
layout (location = 1) out vec4 g_normal;
layout (location = 2) out vec2 g_props;
layout (location = 3) out vec3 g_emissive;

in vec3 uv;

void main() {
    vec4 albedo = getFragmentAlbedo();
    uint shading_model = getMaterialShadingModel();
    vec3 normal = getFragmentNormal();
    float ao = getFragmentAO();
    float metallic = getFragmentMetallic();
    float roughness = getFragmentRoughness();
    vec3 emissive = getFragmentEmissive();

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

//    g_props.r = metallic;
//    g_props.g = roughness;

    g_emissive = emissive;
}
