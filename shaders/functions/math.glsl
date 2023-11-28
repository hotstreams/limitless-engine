/*
 *  Multiples vec3 with mat3 without adding position
 */
vec3 mul_mat3_vec3(const mat4 m, const vec3 v) {
    return v.x * m[0].xyz + (v.y * m[1].xyz + (v.z * m[2].xyz));
}

/*
 *  Multiples vec3 with mat4
 */
vec4 mul_mat4_vec3(const mat4 m, const vec3 v) {
    return v.x * m[0] + (v.y * m[1] + (v.z * m[2] + m[3]));
}

vec3 hash(vec3 a) {
    a = fract(a * 0.8);
    a += dot(a, a.yxz + 19.19);
    return fract((a.xxy + a.yxx) * a.zyx);
}
