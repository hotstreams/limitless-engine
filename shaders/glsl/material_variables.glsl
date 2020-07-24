/*
    extracting material values into writable variables before fragment color computing
    to allow custom material to change this values
*/

#ifdef MATERIAL_COLOR
    vec4 mat_color = material_color;
#endif

#ifdef MATERIAL_EMISSIVE_COLOR
    vec3 mat_emissive_color = material_emissive_color.rgb;
#endif

#ifdef MATERIAL_DIFFUSE
    vec4 mat_diffuse = texture(material_diffuse, fs_data.uv);
#endif

#ifdef MATERIAL_SPECULAR
    float mat_specular = texture(material_specular, fs_data.uv).a;
#else
    float mat_specular = 0.5;
#endif

#ifdef MATERIAL_NORMAL
    vec3 mat_normal = texture(material_normal, fs_data.uv).rgb;
#endif

#ifdef MATERIAL_DISPLACEMENT
    vec3 mat_displacement = texture(material_displacement, fs_data.uv).rgb;
#endif

#ifdef MATERIAL_EMISSIVEMASK
    vec3 mat_emissive_mask = texture(material_emissive_mask, fs_data.uv).rgb;
#endif

#ifdef MATERIAL_BLENDMASK
    float mat_blend_mask = texture(material_blend_mask, fs_data.uv).r;
#endif

#ifdef MATERIAL_SHININESS
    float mat_shininess = material_shininess;
#else
    float mat_shininess = 32.0;
#endif

#ifdef PBR
    #ifdef MATERIAL_METALLIC
        float mat_metallic = material_metallic;
    #else
        float mat_metallic = 0.5;
    #endif

    #ifdef MATERIAL_ROUGHNESS
        float mat_roughness = material_roughness;
    #else
        float mat_roughness = 0.5;
    #endif
#endif
