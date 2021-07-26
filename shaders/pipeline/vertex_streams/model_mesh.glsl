layout (location = 0) in vec3 _vertex_position;
layout (location = 1) in vec3 _vertex_normal;
#if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
    layout (location = 2) in vec3 _vertex_tangent;
#endif
layout (location = 3) in vec2 _vertex_uv;
#if defined (SKELETAL_MODEL)
    layout (location = 4) in ivec4 _vertex_bone_id;
    layout (location = 5) in vec4 _vertex_bone_weight;
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

#if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
    vec3 getVertexTangent() {
        return _vertex_tangent;
    }
#endif

#if defined (SKELETAL_MODEL)
    ivec4 getVertexBoneID() {
        return _vertex_bone_id;
    }

    vec4 getVertexBoneWeight() {
        return _vertex_bone_weight;
    }
#endif
