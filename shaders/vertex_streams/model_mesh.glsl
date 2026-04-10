#if defined (ENGINE_MATERIAL_REGULAR_MODEL) || defined (ENGINE_MATERIAL_SKELETAL_MODEL) || defined (ENGINE_MATERIAL_SKELETAL_INSTANCED_MODEL) || defined (ENGINE_MATERIAL_DECAL_MODEL) || defined (ENGINE_MATERIAL_INSTANCED_MODEL)
    layout (location = 0) in vec3 _vertex_position;
    layout (location = 1) in vec3 _vertex_normal;

    #if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
        layout (location = 2) in vec4 _vertex_tangent; // xyz=tangent, w=handedness
    #endif

    layout (location = 3) in vec2 _vertex_uv;

    // Additional UV sets used as generic payload (SpeedTree wind, etc.)
    // Only declared for non-skeletal models to avoid location conflicts with bone data.
    #if defined (ENGINE_MATERIAL_WIND_MODE) && !defined (ENGINE_MATERIAL_SKELETAL_MODEL) && !defined (ENGINE_MATERIAL_SKELETAL_INSTANCED_MODEL)
        layout (location = 4) in vec2 _vertex_uv1;
        layout (location = 5) in vec2 _vertex_uv2;
        layout (location = 6) in vec2 _vertex_uv3;
        layout (location = 7) in vec2 _vertex_uv4;
        layout (location = 8) in vec2 _vertex_uv5;
    #endif

    #if defined (ENGINE_MATERIAL_SKELETAL_MODEL) || defined (ENGINE_MATERIAL_SKELETAL_INSTANCED_MODEL)
        layout (location = 4) in ivec4 _vertex_bone_id;
        layout (location = 5) in vec4 _vertex_bone_weight;
    #endif
#endif

#if defined (ENGINE_MATERIAL_TERRAIN_MODEL)
    layout (location = 0) in vec3 _vertex_position;
    layout (location = 1) in vec3 _vertex_normal;
    layout (location = 2) in vec4 _vertex_tangent; // xyz=tangent, w=handedness
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

#if defined (ENGINE_MATERIAL_WIND_MODE) && !defined (ENGINE_MATERIAL_SKELETAL_MODEL) && !defined (ENGINE_MATERIAL_SKELETAL_INSTANCED_MODEL)
    vec2 getVertexUv1() { return _vertex_uv1; }
    vec2 getVertexUv2() { return _vertex_uv2; }
    vec2 getVertexUv3() { return _vertex_uv3; }
    vec2 getVertexUv4() { return _vertex_uv4; }
    vec2 getVertexUv5() { return _vertex_uv5; }
#endif

#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
    vec3 getVertexTangent() {
        return _vertex_tangent.xyz;
    }

    // Marker macro so shared code can safely provide a fallback when a vertex stream
    // does not supply tangent handedness.
    #define ENGINE_HAS_VERTEX_TANGENT_SIGN 1

    float getVertexTangentSign() {
        return _vertex_tangent.w;
    }
#endif

#if defined (ENGINE_MATERIAL_SKELETAL_MODEL) || defined (ENGINE_MATERIAL_SKELETAL_INSTANCED_MODEL)
    ivec4 getVertexBoneID() {
        return _vertex_bone_id;
    }

    vec4 getVertexBoneWeight() {
        return _vertex_bone_weight;
    }
#endif
