GraphicsEngine::GLSL_VERSION
GraphicsEngine::Extensions
GraphicsEngine::Settings
GraphicsEngine::MaterialType

#include "glsl/output_data_vs.glsl"
#include "glsl/material.glsl"

layout(location = 0) in vec3 position;
#ifdef MATERIAL_LIT
    layout(location = 1) in vec3 normal;
    #ifdef NORMAL_MAPPING
        layout(location = 2) in vec3 tangent;
    #endif
#endif
layout(location = 3) in vec2 uv;
#ifdef SKELETAL_MODEL_
    #include "glsl/bones.glsl"
    layout (location = 4) in ivec4 bone_id;
    layout (location = 5) in vec4 bone_weight;
#endif

#include "glsl/scene.glsl"

uniform mat4 model;

void main()
{
    fs_data.uv = uv;

    mat4 model_matrix = model;

    #ifdef SKELETAL_MODEL
        mat4 bone_transform = bones[bone_id[0]] * bone_weight[0];
        bone_transform     += bones[bone_id[1]] * bone_weight[1];
        bone_transform     += bones[bone_id[2]] * bone_weight[2];
        bone_transform     += bones[bone_id[3]] * bone_weight[3];

        model_matrix *= bone_transform;
    #endif

    vec4 world_pos = model_matrix * vec4(position, 1.0);

    #ifdef MATERIAL_LIT
        fs_data.world_position = world_pos.xyz;

        #ifdef MATERIAL_NORMAL
            #ifdef NORMAL_MAPPING
                vec3 T = normalize(vec3(model_matrix * vec4(tangent, 0.0)));
                vec3 N = normalize(vec3(model_matrix * vec4(normal, 0.0)));
                T = normalize(T - dot(T, N) * N);
                vec3 B = cross(N, T);
                fs_data.TBN = mat3(T, B, N);
            #endif
        #else
            fs_data.normal = transpose(inverse(mat3(model_matrix))) * normal;
        #endif
    #endif

	gl_Position = VP * world_pos;
}