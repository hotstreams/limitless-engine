struct InstanceData {
    mat4 model_transform;
    vec4 outline_color;
    uint id;
    uint is_outlined;
    uint decal_mask;
    uint pad;
};

// REGULAR MODEL
#if defined (ENGINE_MATERIAL_REGULAR_MODEL) || defined (ENGINE_MATERIAL_SKELETAL_MODEL) || defined (ENGINE_MATERIAL_DECAL_MODEL) || defined (ENGINE_MATERIAL_TERRAIN_MODEL) || (defined (ENGINE_MATERIAL_EFFECT_MODEL) && !defined (MeshEmitter))
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
//

// INSTANCED MODEL
#if defined (ENGINE_MATERIAL_INSTANCED_MODEL)
    layout (std430) buffer model_buffer {
        InstanceData _instances[];
    };

    mat4 getModelMatrix() {
        return _instances[getInstanceId()].model_transform;
    }

    vec3 getOutlineColor() {
        return _instances[getInstanceId()].outline_color.rgb;
    }

    uint getId() {
        return _instances[getInstanceId()].id;
    }

    uint getIsOutlined() {
        return _instances[getInstanceId()].is_outlined;
    }

    uint getDecalMask() {
        return _instances[getInstanceId()].decal_mask;
    }
#endif
//