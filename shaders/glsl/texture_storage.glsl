#extension GL_ARB_bindless_texture : enable
#extension GL_ARB_shader_storage_buffer_object : enable

layout(std430) readonly buffer handle_storage {
    uvec2 handles[];
};