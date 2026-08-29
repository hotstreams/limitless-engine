in vertex_data {
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

    #if defined (ENGINE_MATERIAL_INSTANCED_MODEL)
        flat int instance_id;
    #endif
} in_data;

vec3 getVertexPosition() {
    return in_data.world_position;
}

vec2 getVertexUV() {
    return in_data.uv;
}

#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
    mat3 getVertexTBN() {
        return in_data.TBN;
    }
#else
    vec3 getVertexNormal() {
        return in_data.normal;
    }
#endif

#if defined (ENGINE_MATERIAL_TERRAIN_MODEL)
    vec2 getVertexUV1() {
        return in_data.uv1;
    }

    vec2 getVertexUV2() {
        return in_data.uv2;
    }

    vec2 getVertexUV3() {
        return in_data.uv3;
    }

    uint getVertexColor() {
        return in_data.mask;
    }

    uint getVertexTileCurrent() {
        return in_data.current;
    }

    uint getVertexTileType() {
        return in_data.types;
    }
#endif

#if defined (ENGINE_MATERIAL_INSTANCED_MODEL)
    int getInstanceId() {
        return in_data.instance_id;
    }
#endif
