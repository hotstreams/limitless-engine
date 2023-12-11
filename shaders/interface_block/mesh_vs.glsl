out _vertex_data {
    #if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
        mat3 TBN;
    #else
        vec3 normal;
    #endif

    vec3 world_position;
    vec2 uv;

    #if defined (ENGINE_MATERIAL_TERRAIN_MODEL)
        vec2 uv1;
        vec2 uv2;
        vec2 uv3;
        flat uint mask;
        flat uint current;
        flat uint types;
    #endif
} _out_data;
