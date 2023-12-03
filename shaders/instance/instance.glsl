#include "../vertex_streams/vertex_stream.glsl"

// REGULAR MODEL OR SKELETAL MODEL
#if defined (ENGINE_MATERIAL_REGULAR_MODEL) || defined (ENGINE_MATERIAL_SKELETAL_MODEL) || defined (ENGINE_MATERIAL_DECAL_MODEL)
uniform mat4 _model_transform;

mat4 getModelMatrix() {
    return _model_transform;
}
#endif
//

// INSTANCED MODEL
#if defined (ENGINE_MATERIAL_INSTANCED_MODEL)
layout (std430) buffer model_buffer {
    mat4 _models[];
};

mat4 getModelMatrix() {
    return _models[gl_InstanceID];
}
#endif
//

// SKELETAL MODEL
#if defined (ENGINE_MATERIAL_SKELETAL_MODEL)
layout (std430) buffer bone_buffer {
    mat4 _bones[];
};

mat4 getBoneMatrix() {
    ivec4 bone_id = getVertexBoneID();
    vec4 bone_weight = getVertexBoneWeight();

    mat4 bone_transform = _bones[bone_id[0]] * bone_weight[0];
    bone_transform     += _bones[bone_id[1]] * bone_weight[1];
    bone_transform     += _bones[bone_id[2]] * bone_weight[2];
    bone_transform     += _bones[bone_id[3]] * bone_weight[3];

    return bone_transform;
}
#endif
//

// EFFECT MODEL
#if defined (ENGINE_MATERIAL_EFFECT_MODEL) && !defined (MeshEmitter)
    mat4 getModelMatrix() {
        return mat4(1.0);
    }
#endif

mat4 getModelTransform() {
    #if defined (ENGINE_MATERIAL_SKELETAL_MODEL)
        return getModelMatrix() * getBoneMatrix();
    #else
        return getModelMatrix();
    #endif
}

// TBN matrix only for meshes
#if (defined (MeshEmitter) || defined (ENGINE_MATERIAL_REGULAR_MODEL) || defined (ENGINE_MATERIAL_SKELETAL_MODEL) || defined (ENGINE_MATERIAL_INSTANCED_MODEL) || defined (ENGINE_MATERIAL_DECAL_MODEL)) && defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
    mat3 getModelTBN(mat4 model_transform) {
        //TODO: pass through uniform instance buffer ? bone transform ?
        mat3 normal_matrix = transpose(inverse(mat3(model_transform)));

        vec3 T = normalize(normal_matrix * getVertexTangent());
        vec3 N = normalize(normal_matrix * getVertexNormal());
        T = normalize(T - dot(T, N) * N);
        vec3 B = cross(N, T);

        return mat3(T, B, N);
    }
    //TODO: remove?
    // added because material_context.glsl needed "getVertexTBN" to be compiled in vertex shader
    mat3 getVertexTBN() {
        return getModelTBN(getModelTransform());
    }
#endif
