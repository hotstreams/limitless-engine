vec3 subsurfaceShading(const ShadingContext sctx, const LightingContext lctx, const Light light) {
    vec3 Fd = DiffuseBRDF(sctx, lctx);
    vec3 Fr = vec3(0.0);
    if (lctx.NoL > 0.0) {
        Fr = SpecularBRDF(sctx, lctx) * sctx.energyCompensation;
    }

    vec3 color = (Fd + Fr) * (lctx.NoL * lctx.occlusion);
    float scatterVoH = saturate(dot(sctx.V, -lctx.L));
    float forwardScatter = exp2(scatterVoH * pixel.subsurfacePower - pixel.subsurfacePower);
    float backScatter = saturate(lctx.NoL * sctx.thickness + (1.0 - sctx.thickness)) * 0.5;
    float subsurface = mix(backScatter, 1.0, forwardScatter) * (1.0 - sctx.thickness);
    color += sctx.subsurfaceColor * (subsurface * Fd_Lambert());

    return (color * lctx.color.rgb) * (lctx.color.a * light.attenuation);
}