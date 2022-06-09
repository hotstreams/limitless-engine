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

out vec3 color;

uniform vec3 color_id;

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

    color = color_id;
}
