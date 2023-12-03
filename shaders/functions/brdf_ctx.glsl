vec3 DiffuseBRDF(const ShadingContext sctx, const LightingContext lctx) {
    #if defined (ENGINE_MATERIAL_DIFFUSE_BURLEY)
    return sctx.diffuseColor * Fd_Burley(sctx.roughness, sctx.NoV, lctx.NoL, lctx.LoH);
    #elif defined (ENGINE_MATERIAL_DIFFUSE_LAMBERT)
    return sctx.diffuseColor * Fd_Lambert();
    #endif
}

vec3 SpecularBRDF(const ShadingContext sctx, const LightingContext lctx) {
    float D = D_GGX(sctx.roughness, lctx.NoH);
    #if defined (ENGINE_MATERIAL_SPECULAR_GGX)
    float V = V_SmithGGXCorrelated(sctx.NoV, lctx.NoL, sctx.a2, lctxlambdaV);
    #elif defined (ENGINE_MATERIAL_SPECULAR_GGX_FAST)
    float V = V_SmithGGXCorrelatedFast(sctx.roughness, sctx.NoV, lctx.NoL);
    #endif
    vec3 F = SpecularF(sctx.F0, lctx.LoH);
    return (D * V) * F;
}

vec3 DiffuseBRDF_Cloth(const ShadingContext sctx, const LightingContext lctx) {
    vec3 diffuse = DiffuseBRDF(sctx, lctx);
    #if defined (ENGINE_MATERIAL_SUBSURFACE_COLOR)
    diffuse *= Fd_Wrap(lsctx.NoL, 0.5);
    #endif
    return diffuse;
}

vec3 SpecularBRDF_Cloth(const ShadingContext sctx, const LightingContext lctx) {
    float D = D_Charlie(sctx.roughness, lctx.NoH);
    float V = V_Neubelt(sctx.NoV, lctx.NoL);
    return (D * V) * sctx.F0;
}