layout (location = 0) in vec4 vertex_position;

vec4 getParticlePosition() {
    return vertex_position;
}

#if defined(InitialColor_MODULE)
    layout (location = 1) in vec4 vertex_color;

    vec4 getParticleColor() {
        return vertex_color;
    }
#endif

#if defined(SubUV_MODULE)
    layout(location = 2) in vec4 vertex_subUV;

    vec4 getParticleSubUV() {
        return vertex_subUV;
    }
#endif

#if defined(CustomMaterial_MODULE)
    layout(location = 3) in vec4 vertex_properties;

    vec4 getParticleProperties() {
        return vertex_properties;
    }
#endif

#if defined(Lifetime_MODULE) || defined(Acceleration_MODULE)
    layout(location = 4) in vec4 vertex_acceleration_lifetime;

    vec3 getParticleAcceleration() {
        return vertex_acceleration_lifetime.xyz;
    }

    float getParticleLifetime() {
        return vertex_acceleration_lifetime.w;
    }
#endif

#if defined(InitialRotation_MODULE) || defined(Time_MODULE)
    layout(location = 5) in vec4 vertex_rotation_time;

    vec3 getParticleRotation() {
        return vertex_rotation_time.xyz;
    }

    float getParticleTime() {
        return vertex_rotation_time.w;
    }
#endif

#if defined(InitialVelocity_MODULE) || defined(InitialSize_MODULE)
    layout(location = 6) in vec4 vertex_velocity_size;

    vec3 getParticleVelocity() {
        return vertex_velocity_size.xyz;
    }

    float getParticleSize() {
        return vertex_position_size.w;
    }
#endif

layout (location = 7) in vec2 vertex_uv;

vec2 getParticleUV() {
    return vertex_uv;
}
