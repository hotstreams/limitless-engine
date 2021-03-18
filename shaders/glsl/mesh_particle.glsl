struct MeshParticle {
    mat4 model;

    vec4 color;
    vec4 subUV;
    vec4 properties;
    vec3 acceleration;
    vec3 position;
    vec3 rotation;
    vec3 velocity;
    float lifetime;
    float size;
};

layout (std430) buffer mesh_emitter_particles {
    MeshParticle particles[];
};