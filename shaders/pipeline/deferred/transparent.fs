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

layout (location = 0) out vec3 g_emissive;
layout (location = 1) out vec4 shaded;

#if defined (MATERIAL_REFRACTION)
    uniform sampler2D refraction_texture;

    vec3 getRefractionColor(vec3 N, float refraction, float roughness) {
        //TODO: refactor ?
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
        shaded.rgb = getLitShadingColor(albedo.rgb, ao, normal, getVertexPosition(), getVertexUV(), metallic, roughness);
    #endif

    #if defined (MATERIAL_UNLIT)
        shaded.rgb = getUnlitShadingColor(albedo.rgb, ao, getVertexUV());
    #endif

    shaded.a = albedo.a;

    #if defined (MATERIAL_REFRACTION)
        //TODO: lerp based on transmission
        shaded.rgb += getRefractionColor(normal, refraction, roughness);
    #endif

    g_emissive = emissive;
}
