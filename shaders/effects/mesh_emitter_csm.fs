Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::EmitterType

#include "glsl/material.glsl"
#include "glsl/scene.glsl"

in vec2 fs_uv;
flat in int particle_id;

#include "glsl/mesh_particle.glsl"

void main()
{
    vec2 uv = fs_uv;

    vec3 p_position = particles[particle_id].position;
    vec4 p_color = particles[particle_id].color;
    vec3 p_rotation = particles[particle_id].rotation;
    vec3 p_velocity = particles[particle_id].velocity;
    vec3 p_acceleration = particles[particle_id].acceleration;
    float p_lifetime = particles[particle_id].lifetime;
    float p_size = particles[particle_id].size;
    vec4 p_subUV = particles[particle_id].subUV;
    vec4 p_properties = particles[particle_id].properties;

    #ifdef SubUV_MODULE
        uv = uv * p_subUV.xy + p_subUV.zw;
    #endif

    #include "glsl/material_variables.glsl"

    Limitless::CustomMaterialFragmentCode

    #ifdef MATERIAL_BLENDMASK
        if (mat_blend_mask == 0.0) discard;
    #endif
}