struct FragmentData {
#if defined (MATERIAL_COLOR)
    vec4 color;
#endif

#if defined (MATERIAL_EMISSIVE_COLOR)
    vec3 emissive;
#endif

#if defined (MATERIAL_DIFFUSE)
    vec4 baseColor;
#endif

#if defined (MATERIAL_NORMAL)
    vec3 normal;
#endif

#if defined (MATERIAL_EMISSIVEMASK)
    vec3 emissive_mask;
#endif

#if defined (MATERIAL_BLENDMASK)
    float blend_mask;
#endif

    float metallic;
    float roughness;

#if defined (MATERIAL_AMBIENT_OCCLUSION_TEXTURE) || defined (MATERIAL_ORM_TEXTURE)
    float ao;
#endif

#if defined (MATERIAL_DISPLACEMENT)
    vec3 displacement;
#endif

#if defined (MATERIAL_TESSELLATION_FACTOR)
    vec2 tessellation_factor;
#endif

#if defined (MATERIAL_REFRACTION)
    float IoR;
    float absorption;
#endif

    uint shading_model;
};
