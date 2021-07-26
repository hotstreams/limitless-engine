in _vertex_data {
    #if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
        mat3 TBN;
    #else
        vec3 normal;
    #endif

    vec3 world_position;
    vec2 uv;
} _in_data;

vec3 getVertexPosition() {
    return _in_data.world_position;
}

vec2 getVertexUV() {
    return _in_data.uv;
}

#if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
    mat3 getVertexTBN() {
        return _in_data.TBN;
    }
#else
    vec3 getVertexNormal() {
        return _in_data.normal;
    }
#endif
