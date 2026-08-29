struct InstanceData {
    mat4 model_transform;
    vec4 outline_color;
    uint id;
    uint is_outlined;
    uint decal_mask;
    uint pad;
};

// REGULAR MODEL
#if defined (ENGINE_MATERIAL_REGULAR_MODEL) || defined (ENGINE_MATERIAL_SKELETAL_MODEL) || defined (ENGINE_MATERIAL_DECAL_MODEL) || defined (ENGINE_MATERIAL_TERRAIN_MODEL) || (defined (ENGINE_MATERIAL_EFFECT_MODEL))
    layout (std140) uniform INSTANCE_BUFFER {
        InstanceData instance_data;
    };

    mat4 getModelMatrix() {
        return instance_data.model_transform;
    }

    vec3 getOutlineColor() {
        return instance_data.outline_color.rgb;
    }

    uint getId() {
        return instance_data.id;
    }

    uint getIsOutlined() {
        return instance_data.is_outlined;
    }

    uint getDecalMask() {
        return instance_data.decal_mask;
    }
#endif
//

// INSTANCED MODEL
#if defined (ENGINE_MATERIAL_INSTANCED_MODEL)
    layout (std430) buffer model_buffer {
        InstanceData instances[];
    };

    mat4 getModelMatrix() {
        return instances[getInstanceId()].model_transform;
    }

    vec3 getOutlineColor() {
        return instances[getInstanceId()].outline_color.rgb;
    }

    uint getId() {
        return instances[getInstanceId()].id;
    }

    uint getIsOutlined() {
        return instances[getInstanceId()].is_outlined;
    }

    uint getDecalMask() {
        return instances[getInstanceId()].decal_mask;
    }
#endif
//