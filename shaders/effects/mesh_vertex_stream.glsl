layout(location = 0) in vec3 _position;

vec3 getMeshPosition() {
    return _position;
}

layout(location = 1) in vec3 _normal;

vec3 getMeshNormal() {
    return _normal;
}

#if defined(MATERIAL_LIT)
    #if defined(NORMAL_MAPPING)
        layout(location = 2) in vec3 _tangent;

        vec3 getMeshTangent() {
            return _tangent;
        }
    #endif
#endif

layout(location = 3) in vec2 _uv;

vec2 getMeshUV() {
    return _uv;
}

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

mat4 getParticleModel() {
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
