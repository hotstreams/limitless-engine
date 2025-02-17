mat3 calculateTBN(vec3 normal, vec3 tangent, mat3 normal_matrix) {
    vec3 T = normalize(normal_matrix * tangent);
    vec3 N = normalize(normal_matrix * normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    return mat3(T, B, N);
}