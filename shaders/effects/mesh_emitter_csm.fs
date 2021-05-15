Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::EmitterType

#include "../glsl/material.glsl"
#include "../glsl/scene.glsl"

in vertex_data {
    vec2 uv;
    flat int particle_id;
} in_data;

#include "../glsl/mesh_particle.glsl"

void main() {
    vec2 uv = in_data.uv;
    int particle_id = in_data.particle_id;

    vec3 p_position = particles[particle_id].position;
    vec4 p_color = particles[particle_id].color;
    vec3 p_rotation = particles[particle_id].rotation;
    vec3 p_velocity = particles[particle_id].velocity;
    vec3 p_acceleration = particles[particle_id].acceleration;
    float p_lifetime = particles[particle_id].lifetime;
    float p_size = particles[particle_id].size;
    vec4 p_subUV = particles[particle_id].subUV;
    vec4 p_properties = particles[particle_id].properties;

    #if defined(SubUV_MODULE)
        uv = uv * p_subUV.xy + p_subUV.zw;
    #endif

    #include "../glsl/material_variables.glsl"

    Limitless::CustomMaterialFragmentCode

    #if defined(MATERIAL_BLENDMASK)
        if (mat_blend_mask <= 0.5) discard;
    #endif
}