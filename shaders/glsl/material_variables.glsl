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
    vec4 mat_diffuse = texture(material_diffuse, uv);
#endif

#ifdef MATERIAL_SPECULAR
    float mat_specular = texture(material_specular, uv).a;
#else
    float mat_specular = 0.5;
#endif

#ifdef MATERIAL_NORMAL
    vec3 mat_normal = texture(material_normal, uv).rgb;
#endif

#ifdef MATERIAL_DISPLACEMENT
    vec3 mat_displacement = texture(material_displacement, uv).rgb;
#endif

#ifdef MATERIAL_EMISSIVEMASK
    vec3 mat_emissive_mask = texture(material_emissive_mask, uv).rgb;
#endif

#ifdef MATERIAL_BLENDMASK
    float mat_blend_mask = texture(material_blend_mask, uv).r;
#endif

#ifdef MATERIAL_SHININESS
    float mat_shininess = material_shininess;
#else
    float mat_shininess = 32.0;
#endif

#ifdef PBR
    #ifdef MATERIAL_METALLIC_TEXTURE
        float mat_metallic = texture(material_metallic_texture, uv).r;
    #else
        #ifdef MATERIAL_METALLIC
            float mat_metallic = material_metallic;
        #else
            float mat_metallic = 0.01;
        #endif
    #endif

    #ifdef MATERIAL_ROUGHNESS_TEXTURE
        float mat_roughness = texture(material_roughness_texture, uv).r;
    #else
        #ifdef MATERIAL_ROUGHNESS
            float mat_roughness = material_roughness;
        #else
            float mat_roughness = 0.01;
        #endif
    #endif
#endif
