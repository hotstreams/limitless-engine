layout(std430) buffer bone_buffer {
    #if defined(SKELETAL_INSTANCED_MODEL)
        // bone count per one model
        // needs to calculate the offset in buffer; [bone_count * gl_InstanceID]
        uint bone_count;
    #endif
    mat4 bones[];
};