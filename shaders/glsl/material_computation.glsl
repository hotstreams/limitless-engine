/*
    expects vec4 fragment_color = vec4(1.0) as input to compute
*/

#ifdef MATERIAL_COLOR
    fragment_color *= material_color;
#endif

#ifdef MATERIAL_DIFFUSE
    fragment_color *= mat_diffuse;
#endif

#ifdef MATERIAL_BLENDMASK
    if (mat_blend_mask <= 0.5) discard;
    //fragment_color.a *= mat_blend_mask;
#endif

#ifdef MATERIAL_LIT
    #ifdef MATERIAL_NORMAL
        #ifdef NORMAL_MAPPING
            vec3 normal = normalize(mat_normal * 2.0 - 1.0);
            normal = normalize(fs_data.TBN * normal);
        #else
            vec3 normal = normalize(fs_data.normal);
        #endif
    #else
        vec3 normal = normalize(fs_data.normal);
    #endif
#endif

#ifdef MATERIAL_EMISSIVEMASK
    if (mat_emissive_mask.r != 0.0 && mat_emissive_mask.g != 0.0 && mat_emissive_mask.b != 0.0)
    {
        fragment_color.rgb *= mat_emissive_mask;

        #ifdef MATERIAL_EMISSIVE_COLOR
            fragment_color.rgb *= mat_emissive_color;
        #endif
    }
    else
    {
        #ifdef MATERIAL_LIT
            #ifdef PBR
                fragment_color = vec4(getPBRShadedColor(normal, fragment_color.rgb, mat_metallic, mat_roughness), fragment_color.a);
            #else
                fragment_color = vec4(getShadedColor(fragment_color.rgb, normal, mat_specular, mat_shininess), fragment_color.a);
            #endif
        #endif
    }
#else
    #ifdef MATERIAL_EMISSIVE_COLOR
        fragment_color.rgb *= mat_emissive_color;
    #else
        #ifdef MATERIAL_LIT
            #ifdef PBR
                fragment_color = vec4(getPBRShadedColor(normal, fragment_color.rgb, mat_metallic, mat_roughness), fragment_color.a);
            #else
                fragment_color = vec4(getShadedColor(fragment_color.rgb, normal, mat_specular, mat_shininess), fragment_color.a);
            #endif
        #endif
    #endif
#endif
