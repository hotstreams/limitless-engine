#extension GL_ARB_shader_storage_buffer_object : require
layout(std430) readonly buffer bone_buffer {
    mat4 bones[];
};