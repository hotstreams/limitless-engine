Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::EmitterType

#include "../glsl/material.glsl"
#include "../glsl/scene.glsl"

layout (location = 0) in vec3 position;
layout (location = 3) in vec2 uv;

out vertex_data {
    vec2 uv;
    flat int particle_id;
} out_data;

#include "../glsl/mesh_particle.glsl"

uniform mat4 light_space;

void main() {
    out_data.uv = uv;

    mat4 model = particles[gl_InstanceID].model;
    vec4 vertex_position = vec4(position, 1.0);

    Limitless::CustomMaterialVertexCode

	gl_Position = light_space * model * vertex_position;

	out_data.particle_id = gl_InstanceID;
}



