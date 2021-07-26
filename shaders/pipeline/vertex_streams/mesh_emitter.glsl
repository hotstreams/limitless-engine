#include "./model_mesh.glsl"

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
    MeshParticle _particles[];
};

mat4 getModelMatrix() {
    return _particles[gl_InstanceID].model;
}

vec4 getParticleColor() {
    return _particles[gl_InstanceID].color;
}

vec4 getParticleSubUV() {
    return _particles[gl_InstanceID].subUV;
}

vec4 getParticleProperties() {
    return _particles[gl_InstanceID].properties;
}

vec3 getParticleAcceleration() {
    return _particles[gl_InstanceID].acceleration_lifetime.xyz;
}

float getParticleLifetime() {
    return _particles[gl_InstanceID].acceleration_lifetime.w;
}

vec3 getParticlePosition() {
    return _particles[gl_InstanceID].position.xyz;
}

vec3 getParticleSize() {
    return _particles[gl_InstanceID].size.xyz;
}

vec3 getParticleRotation() {
    return _particles[gl_InstanceID].rotation_time.xyz;
}

float getParticleTime() {
    return _particles[gl_InstanceID].rotation_time.w;
}

vec3 getParticleVelocity() {
    return _particles[gl_InstanceID].velocity.xyz;
}
