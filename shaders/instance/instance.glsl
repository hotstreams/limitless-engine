/*
 *  Instance data storage
 */
struct InstanceContext {
    mat4 model_matrix;
    vec4 outline_color;
    // Local-space AABB of the model (min/max). w unused.
    vec4 aabb_min;
    vec4 aabb_max;
    uint id;
    uint is_outlined;
    uint decal_mask;
    uint pad;
    vec4 lod_fade;
};

/*
 *  Single global model SSBO: regular / instanced / skeletal instanced / terrain (see RendererSettings)
 *  Index: vctx.instance_id = int(gl_BaseInstance) + gl_InstanceID (set in generated vertex shader)
 */
#if defined(ENGINE_GLOBAL_MODEL_INSTANCE_SSBO) && (defined(ENGINE_MATERIAL_REGULAR_MODEL) || defined(ENGINE_MATERIAL_INSTANCED_MODEL) || defined(ENGINE_MATERIAL_SKELETAL_INSTANCED_MODEL) || defined(ENGINE_MATERIAL_TERRAIN_MODEL))

layout (std430) buffer model_buffer {
    InstanceContext _instance_contexts[];
};

InstanceContext computeInstanceContext(const VertexContext vctx) {
    return _instance_contexts[vctx.instance_id];
}

#endif

/*
 *  Instance Uniform Buffer for single draw (legacy, or skeletal/decal/effect)
 */
#if (defined(ENGINE_MATERIAL_REGULAR_MODEL) && !defined(ENGINE_GLOBAL_MODEL_INSTANCE_SSBO)) \
    || defined(ENGINE_MATERIAL_SKELETAL_MODEL) || defined(ENGINE_MATERIAL_DECAL_MODEL) \
    || (defined(ENGINE_MATERIAL_TERRAIN_MODEL) && !defined(ENGINE_GLOBAL_MODEL_INSTANCE_SSBO)) \
    || defined(ENGINE_MATERIAL_EFFECT_MODEL)

layout (std140) uniform INSTANCE_BUFFER {
    InstanceContext _instance_context;
};

InstanceContext computeInstanceContext(const VertexContext vctx) {
    return _instance_context;
}

#endif


/*
 *  Instance Buffer for instanced models (per-draw SSBO, legacy without global pack)
 */
#if defined(ENGINE_MATERIAL_INSTANCED_MODEL) && !defined(ENGINE_GLOBAL_MODEL_INSTANCE_SSBO)

layout (std430) buffer model_buffer {
    InstanceContext _instance_contexts[];
};

InstanceContext computeInstanceContext(const VertexContext vctx) {
    return _instance_contexts[vctx.instance_id];
}

#endif


/*
 *  Instance Buffer and API for `indirect draw models`
 *
 *  Uses gl_BaseInstance + gl_InstanceID for instance data indexing
 *  Uses gl_DrawID (from ARB_shader_draw_parameters) for per-draw resource indexing
 */
#if defined (ENGINE_MATERIAL_INDIRECT_MODEL)

layout (std430) buffer indirect_instance_buffer {
    InstanceContext _indirect_instances[];
};

InstanceContext computeInstanceContext(const VertexContext vctx) {
    // vctx.instance_id is already computed as gl_BaseInstance + gl_InstanceID in vertex shader
    return _indirect_instances[vctx.instance_id];
}

// Returns the draw ID for indexing per-draw resources (textures)
int getDrawId(const VertexContext vctx) {
    return vctx.draw_id;
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
 *  Skeletal instanced: model_buffer + packed bone_buffer (legacy), or bone only when using global model SSBO
 */
#if defined (ENGINE_MATERIAL_SKELETAL_INSTANCED_MODEL) && !defined(ENGINE_GLOBAL_MODEL_INSTANCE_SSBO)

layout (std430) buffer model_buffer {
    InstanceContext _instance_contexts[];
};

InstanceContext computeInstanceContext(const VertexContext vctx) {
    return _instance_contexts[vctx.instance_id];
}

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

#if defined(ENGINE_MATERIAL_SKELETAL_INSTANCED_MODEL) && defined(ENGINE_GLOBAL_MODEL_INSTANCE_SSBO)

layout (std430) buffer bone_buffer {
    uint _bone_count;
    mat4 _bones[];
};

mat4 getBoneMatrix(const VertexContext vctx) {
    ivec4 bone_id = vctx.bone_index;
    vec4 bone_weight = vctx.bone_weight;
    uint bone_offset = uint(vctx.instance_id) * _bone_count;

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
