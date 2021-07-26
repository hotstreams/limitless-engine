layout (std430) buffer model_buffer {
    mat4 _models[];
};

mat4 getModelMatrix() {
    return _models[gl_InstanceID];
}
