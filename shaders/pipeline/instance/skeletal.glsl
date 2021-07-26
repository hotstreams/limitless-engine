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
