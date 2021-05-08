/*
    expects vec4 fragment_color = vec4(1.0) as input to compute
*/

#if defined(MATERIAL_COLOR)
    fragment_color *= mat_color;
#endif

#if defined(MATERIAL_DIFFUSE)
    fragment_color *= mat_diffuse;
#endif

#if defined(MATERIAL_BLENDMASK)
    if (mat_blend_mask <= 0.5) discard;
    //fragment_color.a *= mat_blend_mask;
#endif

#if defined(MATERIAL_LIT)
    #if defined(MATERIAL_NORMAL) && defined(NORMAL_MAPPING)
        vec3 normal = mat_normal * 2.0 - 1.0;
        normal = normalize(in_data.TBN * normal);
    #else
        vec3 normal = normalize(in_data.normal);
    #endif
#endif

#if defined(MATERIAL_EMISSIVEMASK)
    if (mat_emissive_mask.rgb != vec3(0.0)) {
        fragment_color.rgb *= mat_emissive_mask;

        #if defined(MATERIAL_EMISSIVE_COLOR)
            fragment_color.rgb *= mat_emissive_color;
        #endif
    } else {
        #if defined(MATERIAL_LIT)
            #if defined(PBR) && (defined(MATERIAL_METALLIC_TEXTURE) || defined(MATERIAL_METALLIC)) && (defined(MATERIAL_ROUGHNESS_TEXTURE) || defined(MATERIAL_ROUGHNESS))
                fragment_color = vec4(getPBRShadedColor(normal, fragment_color.rgb, mat_metallic, mat_roughness), fragment_color.a);
            #else
                fragment_color = vec4(getShadedColor(fragment_color.rgb, normal, mat_specular, mat_shininess), fragment_color.a);
            #endif
        #endif
    }
#else
    #if defined(MATERIAL_EMISSIVE_COLOR)
        fragment_color.rgb *= mat_emissive_color;
    #else
        #if defined(MATERIAL_LIT)
            #if defined(PBR) && (defined(MATERIAL_METALLIC_TEXTURE) || defined(MATERIAL_METALLIC)) && (defined(MATERIAL_ROUGHNESS_TEXTURE) || defined(MATERIAL_ROUGHNESS))
                fragment_color = vec4(getPBRShadedColor(normal, fragment_color.rgb, mat_metallic, mat_roughness), fragment_color.a);
            #else
                fragment_color = vec4(getShadedColor(fragment_color.rgb, normal, mat_specular, mat_shininess), fragment_color.a);
            #endif
        #endif
    #endif
#endif
