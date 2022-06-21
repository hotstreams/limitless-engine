FragmentData initializeFragmentData() {
    FragmentData data;

#if defined (MATERIAL_COLOR)
    data.color = getMaterialColor();
#endif

#if defined (MATERIAL_EMISSIVE_COLOR)
    data.emissive = getMaterialEmissive();
#endif

#if defined (MATERIAL_DIFFUSE)
    data.baseColor = getMaterialDiffuse(getVertexUV());
#endif

#if defined (MATERIAL_NORMAL)
    data.normal = getMaterialNormal(getVertexUV());
#endif

#if defined (MATERIAL_EMISSIVEMASK)
    data.emissive_mask = getMaterialEmissiveMask(getVertexUV());
#endif

#if defined (MATERIAL_BLENDMASK)
    data.blend_mask = getMaterialBlendMask(getVertexUV());
#endif

#if defined (MATERIAL_METALLIC_TEXTURE)
    data.metallic = getMaterialMetallic(getVertexUV());
#elif defined (MATERIAL_METALLIC)
    data.metallic = getMaterialMetallic();
#else
    data.metallic = 0.6;
#endif

#if defined (MATERIAL_ROUGHNESS_TEXTURE)
    data.roughness = getMaterialRoughness(getVertexUV());
#elif defined (MATERIAL_ROUGHNESS)
    data.roughness = getMaterialRoughness();
#else
    data.roughness = 0.3;
#endif

#if defined (MATERIAL_AMBIENT_OCCLUSION_TEXTURE)
    data.ao = getMaterialAmbientOcclusion(getVertexUV());
#endif

#if defined (MATERIAL_DISPLACEMENT)
data.displacement = getMaterialDisplacement(getVertexUV());
    #endif

#if defined (MATERIAL_TESSELLATION_FACTOR)
    data.tessellation_factor = getMaterialTesselationFactor();
#endif

#if defined (MATERIAL_REFRACTION)
    #if defined (MATERIAL_IOR)
        data.IoR = getMaterialIOR();
    #else
        data.IoR = 1.0;
    #endif

    #if defined (MATERIAL_ABSORPTION)
        data.absorption = getMaterialAbsorption();
    #else
        data.absorption = 0.0;
    #endif
#endif

    data.shading_model = getMaterialShadingModel();

    return data;
}
