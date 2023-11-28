#include "../functions/common.glsl"
#include "../pipeline/scene.glsl"

#define ENGINE_SHADING_UNLIT 0u
#define ENGINE_SHADING_LIT 1u
#define ENGINE_SHADING_CUSTOM 3u

struct ShadingContext {
    vec3 diffuseColor;
    float metallic;

    vec3 worldPos;
    float roughness;

    vec3 N;
    float a2;

    vec3 V;
    float NoV;

    vec3 F0;
    float ambientOcclusion;

    vec3 emissive_color;
    float alpha;

    vec3 indirect_lighting;
    uint shading_model;
};

ShadingContext computeShadingContext(
    const vec3 baseColor,
    float alpha,
    float metallic,
    const vec3 worldPos,
    float roughness,
    const vec3 N,
    float ambientOcclusion,
    vec3 emissive_color,
    uint shading_model,
    vec3 indirect_lighting
) {
    ShadingContext context;

    context.metallic = saturate(metallic);
    context.diffuseColor = computeDiffuseColor(baseColor, context.metallic);
    context.alpha = alpha;
    context.shading_model = shading_model;

    context.worldPos = worldPos;

    float perceptualRoughness = roughness;

#if defined (ENGINE_SETTINGS_SPECULAR_AA)
    perceptualRoughness = specularAA(N, perceptualRoughness, ENGINE_SETTINGS_SPECULAR_AA_THRESHOLD, ENGINE_SETTINGS_SPECULAR_AA_VARIANCE);
#endif

    perceptualRoughness = clamp(perceptualRoughness, MIN_PERCEPTUAL_ROUGHNESS, 1.0);
    context.roughness = perceptualRoughness * perceptualRoughness;

    context.N = N;
    context.a2 = context.roughness * context.roughness;

    context.V = normalize(getCameraPosition() - context.worldPos);
    context.NoV = computeNoV(context.N, context.V);

    context.F0 = computeF0(baseColor, context.metallic, 1.0);
    context.ambientOcclusion = ambientOcclusion;
    context.emissive_color = emissive_color;

#if defined (ENGINE_MATERIAL_ANISOTROPY)
    vec3 direction = material.anisotropyDirection;
    pixel.anisotropy = material.anisotropy;
    pixel.anisotropicT = normalize(shading_tangentToWorld * direction);
    pixel.anisotropicB = normalize(cross(getWorldGeometricNormalVector(), pixel.anisotropicT));
#endif

    context.indirect_lighting = indirect_lighting;

    return context;
}