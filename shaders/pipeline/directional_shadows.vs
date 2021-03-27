LimitlessEngine::GLSL_VERSION
LimitlessEngine::Extensions
LimitlessEngine::Settings
LimitlessEngine::MaterialType
LimitlessEngine::ModelType

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
#ifdef SKELETAL_MODEL
    #include "glsl/bones.glsl"
    layout (location = 4) in ivec4 bone_id;
    layout (location = 5) in vec4 bone_weight;
#endif

#include "glsl/scene.glsl"

uniform mat4 model;
uniform mat4 light_space;

void main()
{
    fs_data.uv = uv;

    mat4 model_matrix = model;
    vec4 vertex_position = vec4(position, 1.0);

    #ifdef CUSTOM_MATERIAL
        LimitlessEngine::CustomMaterialVertexCode
    #endif

    #ifdef SKELETAL_MODEL
        mat4 bone_transform = bones[bone_id[0]] * bone_weight[0];
        bone_transform     += bones[bone_id[1]] * bone_weight[1];
        bone_transform     += bones[bone_id[2]] * bone_weight[2];
        bone_transform     += bones[bone_id[3]] * bone_weight[3];

        model_matrix *= bone_transform;
    #endif

    vec4 world_pos = model_matrix * vertex_position;

    #ifdef MATERIAL_LIT
        fs_data.world_position = world_pos.xyz;

        #ifdef MATERIAL_NORMAL
            #ifdef NORMAL_MAPPING
                mat3 normal_matrix = transpose(inverse(mat3(model_matrix)));
                vec3 T = normalize(normal_matrix * tangent);
                vec3 N = normalize(normal_matrix * normal);
                T = normalize(T - dot(T, N) * N);
                vec3 B = cross(N, T);

                fs_data.TBN = mat3(T, B, N);
            #else
                fs_data.normal = transpose(inverse(mat3(model_matrix))) * normal;
            #endif
        #else
            fs_data.normal = transpose(inverse(mat3(model_matrix))) * normal;
        #endif
    #endif

    gl_Position = light_space * world_pos;
}