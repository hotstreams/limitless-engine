/*
 *  Instance data storage
 */
struct InstanceData {
    mat4 model_transform;
    vec4 outline_color;
    uint id;
    uint is_outlined;
    uint decal_mask;
    uint pad;
};

/*
 *  Instance Uniform Buffer and API for `single model`
 */
#if defined (ENGINE_MATERIAL_REGULAR_MODEL) || defined (ENGINE_MATERIAL_SKELETAL_MODEL) || defined (ENGINE_MATERIAL_DECAL_MODEL) || defined (ENGINE_MATERIAL_TERRAIN_MODEL)

layout (std140) uniform INSTANCE_BUFFER {
    InstanceData _instance_data;
};

mat4 getModelMatrix() {
    return _instance_data.model_transform;
}

vec3 getOutlineColor() {
    return _instance_data.outline_color.rgb;
}

uint getId() {
    return _instance_data.id;
}

uint getIsOutlined() {
    return _instance_data.is_outlined;
}

uint getDecalMask() {
    return _instance_data.decal_mask;
}

#endif


/*
 *  Instance Buffer and API for `instanced models`
 */
#if defined (ENGINE_MATERIAL_INSTANCED_MODEL)

layout (std430) buffer model_buffer {
    InstanceData _instances[];
};

mat4 getModelMatrix() {
    return _instances[gl_InstanceID].model_transform;
}

vec3 getOutlineColor() {
    return _instances[gl_InstanceID].outline_color.rgb;
}

uint getId() {
    return _instances[gl_InstanceID].id;
}

uint getIsOutlined() {
    return _instances[gl_InstanceID].is_outlined;
}

uint getDecalMask() {
    return _instances[gl_InstanceID].decal_mask;
}

#endif


/*
 *  Skeletal Buffer for skeletal instance
 */
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


/*
 *  Skeletal buffer for skeletal instanced instance
 */
#if defined (ENGINE_MATERIAL_SKELETAL_INSTANCED_MODEL)

layout (std430) buffer bone_buffer {
    uint _bone_count;
    mat4 _bones[];
};

mat4 getBoneMatrix() {
    ivec4 bone_id = getVertexBoneID();
    vec4 bone_weight = getVertexBoneWeight();
    uint bone_offset = gl_InstanceID * _bone_count;

    mat4 bone_transform = _bones[bone_offset + bone_id[0]] * bone_weight[0];
    bone_transform     += _bones[bone_offset + bone_id[1]] * bone_weight[1];
    bone_transform     += _bones[bone_offset + bone_id[2]] * bone_weight[2];
    bone_transform     += _bones[bone_offset + bone_id[3]] * bone_weight[3];

    return bone_transform;
}

#endif

mat4 getModelTransform() {
#if defined (ENGINE_MATERIAL_SKELETAL_MODEL) || defined (ENGINE_MATERIAL_SKELETAL_INSTANCED_MODEL)
    return getModelMatrix() * getBoneMatrix();
#else
    return getModelMatrix();
#endif
}

// EFFECT MODEL
#if defined (ENGINE_MATERIAL_EFFECT_MODEL) && !defined (MeshEmitter)
    mat4 getModelMatrix() {
        return mat4(1.0);
    }
#endif

// TBN matrix only for meshes
#if (defined (MeshEmitter) || defined (ENGINE_MATERIAL_REGULAR_MODEL) || defined (ENGINE_MATERIAL_SKELETAL_MODEL) || defined (ENGINE_MATERIAL_INSTANCED_MODEL) || defined (ENGINE_MATERIAL_DECAL_MODEL) || defined (ENGINE_MATERIAL_TERRAIN_MODEL)) && defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
    mat3 getModelTBN(mat4 model_transform) {
        //TODO: pass through uniform instance buffer ? bone transform ?
        //TODO: research on transpose inverse bone scaling
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
