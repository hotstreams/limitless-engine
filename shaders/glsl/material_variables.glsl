/*
    extracting material values into writable variables before fragment color computing
    to allow custom material to change this values
*/

#if defined(MATERIAL_COLOR)
    vec4 mat_color = material_color;
#endif

#if defined(MATERIAL_EMISSIVE_COLOR)
    vec3 mat_emissive_color = material_emissive_color.rgb;
#endif

#if defined(MATERIAL_DIFFUSE)
    vec4 mat_diffuse = texture(material_diffuse, uv);
#endif

#if defined(MATERIAL_SPECULAR)
    float mat_specular = texture(material_specular, uv).a;
#else
    float mat_specular = 0.1;
#endif

#if defined(MATERIAL_NORMAL)
    vec3 mat_normal = texture(material_normal, uv).rgb;
#endif

#if defined(MATERIAL_DISPLACEMENT)
    vec3 mat_displacement = texture(material_displacement, uv).rgb;
#endif

#if defined(MATERIAL_EMISSIVEMASK)
    vec3 mat_emissive_mask = texture(material_emissive_mask, uv).rgb;
#endif

#if defined(MATERIAL_BLENDMASK)
    float mat_blend_mask = texture(material_blend_mask, uv).r;
#endif

#if defined(MATERIAL_SHININESS)
    float mat_shininess = material_shininess;
#else
    float mat_shininess = 8.0;
#endif

#if defined(MATERIAL_TESSELATION_FACTOR)
    vec2 mat_tesselation_factor = material_tesselation_factor;
#endif

#if defined(PBR)
    #if defined(MATERIAL_METALLIC_TEXTURE)
        float mat_metallic = texture(material_metallic_texture, uv).r;
    #else
        #if defined(MATERIAL_METALLIC)
            float mat_metallic = material_metallic;
        #endif
    #endif

    #if defined(MATERIAL_ROUGHNESS_TEXTURE)
        float mat_roughness = texture(material_roughness_texture, uv).r;
    #else
        #if defined(MATERIAL_ROUGHNESS)
            float mat_roughness = material_roughness;
        #endif
    #endif
#endif
