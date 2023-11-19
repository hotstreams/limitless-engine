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
};

ShadingContext computeShadingContext(
    const vec3 baseColor, float metallic,
    const vec3 worldPos, float roughness,
    const vec3 N, float ambientOcclusion
) {
    ShadingContext context;

    context.metallic = saturate(metallic);
    context.diffuseColor = computeDiffuseColor(baseColor, context.metallic);

    context.worldPos = worldPos;

    float perceptualRoughness = clamp(roughness, MIN_PERCEPTUAL_ROUGHNESS, 1.0);
    context.roughness = perceptualRoughness * perceptualRoughness;

    context.N = N;
    context.a2 = context.roughness * context.roughness;

    context.V = normalize(getCameraPosition() - context.worldPos);
    context.NoV = computeNoV(context.N, context.V);

    context.F0 = computeF0(baseColor, context.metallic, 1.0);
    context.ambientOcclusion = ambientOcclusion;

#if defined (ENGINE_MATERIAL_ANISOTROPY)
    vec3 direction = material.anisotropyDirection;
    pixel.anisotropy = material.anisotropy;
    pixel.anisotropicT = normalize(shading_tangentToWorld * direction);
    pixel.anisotropicB = normalize(cross(getWorldGeometricNormalVector(), pixel.anisotropicT));
#endif

    return context;
}

ShadingContext computeShadingContext() {

}
