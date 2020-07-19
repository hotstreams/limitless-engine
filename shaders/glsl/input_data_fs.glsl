in shader_data {
    #ifdef MATERIAL_LIT
        vec3 world_position;
        vec3 normal;
    #endif

    #ifdef NORMAL_MAPPING
        mat3 TBN;
    #endif

    vec2 uv;
} fs_data;