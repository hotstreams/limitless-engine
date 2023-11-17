vec3 clothShading(const ShadingContext sctx, const LightingContext lctx, const Light light) {
    vec3 Fd = DiffuseBRDF_Cloth(sctx, lctx);
    vec3 Fr = SpecularBRDF_Cloth(sctx, lctx);

#if defined (ENGINE_MATERIAL_SUBSURFACE_COLOR)
    Fd *= saturate(sctx.subsurfaceColor + lctx.NoL);
    vec3 color = Fd + Fr * NoL;
    color *= light.color.rgb * (light.color.a * light.attenuation * lctx.visibility);
#else
    vec3 color = Fd + Fr;
    color *= light.color.rgb * (light.color.a * light.attenuation * lctx.NoL * lctx.visibility);
#endif

    return color;
}