out _vertex_data {
    #if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
        mat3 TBN;
    #else
        vec3 normal;
    #endif

    vec3 world_position;
    vec2 uv;
} _out_data;
