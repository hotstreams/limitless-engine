layout (location = 0) in vec3 _vertex_position;
layout (location = 1) in vec3 _vertex_normal;
#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
    layout (location = 2) in vec3 _vertex_tangent;
#endif
layout (location = 3) in vec2 _vertex_uv;
#if defined (ENGINE_MATERIAL_SKELETAL_MODEL)
    layout (location = 4) in ivec4 _vertex_bone_id;
    layout (location = 5) in vec4 _vertex_bone_weight;
#endif
#if defined (ENGINE_MATERIAL_TERRAIN_MODEL)
    layout (location = 4) in vec2 _vertex_uv1;
    layout (location = 5) in vec2 _vertex_uv2;
    layout (location = 6) in vec2 _vertex_uv3;
    layout (location = 7) in uint _vertex_current;
    layout (location = 8) in uint _vertex_mask;
    layout (location = 9) in uint _vertex_types;
#endif

vec3 getVertexPosition() {
    return _vertex_position;
}

vec3 getVertexNormal() {
    return _vertex_normal;
}

vec2 getVertexUV() {
    return _vertex_uv;
}

#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
    vec3 getVertexTangent() {
        return _vertex_tangent;
    }
#endif

#if defined (ENGINE_MATERIAL_SKELETAL_MODEL)
    ivec4 getVertexBoneID() {
        return _vertex_bone_id;
    }

    vec4 getVertexBoneWeight() {
        return _vertex_bone_weight;
    }
#endif

#if defined (ENGINE_MATERIAL_TERRAIN_MODEL)
    vec2 getVertexUV1() {
        return _vertex_uv1;
    }

    vec2 getVertexUV2() {
        return _vertex_uv2;
    }

    vec2 getVertexUV3() {
        return _vertex_uv3;
    }

    uint getVertexColor() {
        return _vertex_mask;
    }

    uint getVertexTileCurrent() {
        return _vertex_current;
    }

    uint getVertexTileType() {
        return _vertex_types;
    }
#endif
