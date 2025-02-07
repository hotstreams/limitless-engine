#if defined (ENGINE_MATERIAL_REGULAR_MODEL) || defined (ENGINE_MATERIAL_SKELETAL_MODEL) || defined (ENGINE_MATERIAL_DECAL_MODEL) || defined (ENGINE_MATERIAL_INSTANCED_MODEL)
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
#endif

#if defined (ENGINE_MATERIAL_TERRAIN_MODEL)
    layout (location = 0) in vec3 _vertex_position;
    layout (location = 1) in vec3 _vertex_normal;
    layout (location = 2) in vec3 _vertex_tangent;
    layout (location = 3) in vec2 _vertex_uv;
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
