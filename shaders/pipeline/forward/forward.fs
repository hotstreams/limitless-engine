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
#include "../lighting/lighting.glsl"

out vec4 color;

#if defined (MATERIAL_REFRACTION)
    uniform sampler2D refraction_texture;

    vec3 getRefractionColor(vec3 N, float refraction, float roughness) {
        const vec2 NV = (mat3(getView()) * N).xy;
        vec2 uv_r = getVertexUV() + NV * refraction;
        return texture(refraction_texture, uv_r).rgb;
    }
#endif

void main() {
    vec4 albedo = getFragmentAlbedo();
    vec3 normal = getFragmentNormal();
    float ao = getFragmentAO();
    float metallic = getFragmentMetallic();
    float roughness = getFragmentRoughness();
    vec3 emissive = getFragmentEmissive();

    #if defined (MATERIAL_REFRACTION)
        float refraction = getMaterialRefraction();
    #endif

    _MATERIAL_FRAGMENT_SNIPPET

    #if defined (MATERIAL_LIT)
        color.rgb = getLitShadingColor(albedo.rgb, ao, normal, getVertexPosition(), getVertexUV(), metallic, roughness);
    #endif

    #if defined (MATERIAL_UNLIT)
        color.rgb = getUnlitShadingColor(albedo.rgb, ao, getVertexUV());
    #endif

    color.rgb += emissive;
}