#if defined (SIMPLE_MODEL) || defined (SKELETAL_MODEL)
    #include "./model.glsl"
#endif

#if defined (SKELETAL_MODEL)
    #include "./skeletal.glsl"
#endif

#if defined (INSTANCED_MODEL)
    #include "./instanced.glsl"
#endif

#if defined (EFFECT_MODEL) && !defined (MeshEmitter)
    mat4 getModelMatrix() {
        return mat4(1.0);
    }
#endif

mat4 getModelTransform() {
    #if defined (SKELETAL_MODEL)
        return getModelMatrix() * getBoneMatrix();
    #else
        return getModelMatrix();
    #endif
}

// TBN matrix only for meshes
#if (defined (MeshEmitter) || defined (SIMPLE_MODEL) || defined (SKELETAL_MODEL) || defined (INSTANCED_MODEL)) && defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
        mat3 getModelTBN(mat4 model_transform) {
            //TODO: pass through uniform instance buffer ? bone transform ?
            mat3 normal_matrix = transpose(inverse(mat3(model_transform)));

            vec3 T = normalize(normal_matrix * getVertexTangent());
            vec3 N = normalize(normal_matrix * getVertexNormal());
            T = normalize(T - dot(T, N) * N);
            vec3 B = cross(N, T);

            return mat3(T, B, N);
        }
#endif
