layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
    layout (location = 2) in vec3 vertex_tangent;
#endif
layout (location = 3) in vec2 vertex_uv;
#if defined (ENGINE_MATERIAL_SKELETAL_MODEL)
    layout (location = 4) in ivec4 vertex_bone_id;
layout (location = 5) in vec4 vertex_bone_weight;
#endif

vec3 getVertexPosition() {
    return vertex_position;
}

vec3 getVertexNormal() {
    return vertex_normal;
}

vec2 getVertexUV() {
    return vertex_uv;
}

#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
    vec3 getVertexTangent() {
    return vertex_tangent;
}
#endif

#if defined (ENGINE_MATERIAL_SKELETAL_MODEL)
    ivec4 getVertexBoneID() {
    return vertex_bone_id;
}

vec4 getVertexBoneWeight() {
    return vertex_bone_weight;
}
#endif

struct MeshParticle {
    mat4 model;
    vec4 color;
    vec4 subUV;
    vec4 properties;
    vec4 acceleration_lifetime;
    vec4 position;
    vec4 rotation_time;
    vec4 velocity;
    vec4 size;
};

layout (std430) buffer mesh_emitter_particles {
    MeshParticle particles[];
};

mat4 getModelMatrix() {
    return particles[gl_InstanceID].model;
}

vec4 getParticleColor() {
    return particles[gl_InstanceID].color;
}

vec4 getParticleSubUV() {
    return particles[gl_InstanceID].subUV;
}

vec4 getParticleProperties() {
    return particles[gl_InstanceID].properties;
}

vec3 getParticleAcceleration() {
    return particles[gl_InstanceID].acceleration_lifetime.xyz;
}

float getParticleLifetime() {
    return particles[gl_InstanceID].acceleration_lifetime.w;
}

vec3 getParticlePosition() {
    return particles[gl_InstanceID].position.xyz;
}

vec3 getParticleSize() {
    return particles[gl_InstanceID].size.xyz;
}

vec3 getParticleRotation() {
    return particles[gl_InstanceID].rotation_time.xyz;
}

float getParticleTime() {
    return particles[gl_InstanceID].rotation_time.w;
}

vec3 getParticleVelocity() {
    return particles[gl_InstanceID].velocity.xyz;
}
