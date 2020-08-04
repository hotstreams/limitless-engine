out shader_data {
    #ifdef MATERIAL_LIT
        vec3 world_position;

        #ifdef MATERIAL_NORMAL
            #ifdef NORMAL_MAPPING
                mat3 TBN;
            #else
                vec3 normal;
            #endif
        #else
            vec3 normal;
        #endif
    #endif

    vec2 uv;
} fs_data;