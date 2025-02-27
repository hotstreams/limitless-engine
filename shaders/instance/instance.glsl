/*
 *  Instance data storage
 */
struct InstanceContext {
    mat4 model_matrix;
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
    InstanceContext _instance_context;
};

InstanceContext computeInstanceContext(const VertexContext vctx) {
    return _instance_context;
}

#endif


/*
 *  Instance Buffer and API for `instanced models`
 */
#if defined (ENGINE_MATERIAL_INSTANCED_MODEL)

layout (std430) buffer model_buffer {
    InstanceContext _instance_contexts[];
};

InstanceContext computeInstanceContext(const VertexContext vctx) {
    return _instance_contexts[vctx.instance_id];
}

#endif


/*
 *  Skeletal Buffer for skeletal instance
 */
#if defined (ENGINE_MATERIAL_SKELETAL_MODEL)

layout (std430) buffer bone_buffer {
    mat4 _bones[];
};

mat4 getBoneMatrix(const VertexContext vctx) {
    ivec4 bone_id = vctx.bone_index;
    vec4 bone_weight = vctx.bone_weight;

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

mat4 getBoneMatrix(const VertexContext vctx) {
    ivec4 bone_id = vctx.bone_index;
    vec4 bone_weight = vctx.bone_weight;
    uint bone_offset = vctx.instance_id * _bone_count;

    mat4 bone_transform = _bones[bone_offset + bone_id[0]] * bone_weight[0];
    bone_transform     += _bones[bone_offset + bone_id[1]] * bone_weight[1];
    bone_transform     += _bones[bone_offset + bone_id[2]] * bone_weight[2];
    bone_transform     += _bones[bone_offset + bone_id[3]] * bone_weight[3];

    return bone_transform;
}

#endif

mat4 getModelTransform(const InstanceContext ictx, const VertexContext vctx) {
#if defined (ENGINE_MATERIAL_SKELETAL_MODEL) || defined (ENGINE_MATERIAL_SKELETAL_INSTANCED_MODEL)
    return ictx.model_matrix * getBoneMatrix(vctx);
#else
    return ictx.model_matrix;
#endif
}