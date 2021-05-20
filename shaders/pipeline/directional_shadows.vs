Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::ModelType

out vertex_data {
    #if defined(MATERIAL_LIT) || defined(MATERIAL_TESSELLATION_FACTOR)
        vec3 world_position;
    #endif

    #if defined(MATERIAL_LIT)
        #if defined(MATERIAL_NORMAL) && defined(NORMAL_MAPPING)
            mat3 TBN;
        #else
            vec3 normal;
        #endif
    #endif

    vec2 uv;
} out_data;

#include "../glsl/material.glsl"

layout(location = 0) in vec3 position;
#if defined(MATERIAL_LIT)
    layout(location = 1) in vec3 normal;
    #if defined(NORMAL_MAPPING)
        layout(location = 2) in vec3 tangent;
    #endif
#endif
layout(location = 3) in vec2 uv;
#if defined(SKELETAL_MODEL)
    #include "../glsl/bones.glsl"
    layout (location = 4) in ivec4 bone_id;
    layout (location = 5) in vec4 bone_weight;
#endif

#include "../glsl/scene.glsl"

#if defined(INSTANCED_MODEL)
    #include "../glsl/instanced_buffer.glsl"
#else
    uniform mat4 model;
#endif

uniform mat4 light_space;

void main() {
    out_data.uv = uv;

    #if defined(INSTANCED_MODEL) || defined(SKELETAL_INSTANCED_MODEL)
        mat4 model_matrix = models[gl_InstanceID];
    #else
        mat4 model_matrix = model;
    #endif

    vec4 vertex_position = vec4(position, 1.0);

    Limitless::CustomMaterialVertexCode

    #if defined(SKELETAL_MODEL)
        mat4 bone_transform = bones[bone_id[0]] * bone_weight[0];
        bone_transform     += bones[bone_id[1]] * bone_weight[1];
        bone_transform     += bones[bone_id[2]] * bone_weight[2];
        bone_transform     += bones[bone_id[3]] * bone_weight[3];

        model_matrix *= bone_transform;
    #endif

    vec4 world_pos = model_matrix * vertex_position;

    #if defined(MATERIAL_LIT) || defined(MATERIAL_TESSELLATION_FACTOR)
        out_data.world_position = world_pos.xyz;
    #endif

    #if defined(MATERIAL_LIT)
        #if defined(MATERIAL_NORMAL) && defined(NORMAL_MAPPING)
            mat3 normal_matrix = transpose(inverse(mat3(model_matrix)));
            vec3 T = normalize(normal_matrix * tangent);
            vec3 N = normalize(normal_matrix * normal);
            T = normalize(T - dot(T, N) * N);
            vec3 B = cross(N, T);

            out_data.TBN = mat3(T, B, N);
        #else
            out_data.normal = transpose(inverse(mat3(model_matrix))) * normal;
        #endif
    #endif

    #if !defined(MATERIAL_TESSELATION_FACTOR)
        gl_Position = light_space * world_pos;
    #endif
}