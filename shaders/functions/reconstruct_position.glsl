vec3 reconstructPosition(vec2 uv, float depth, mat4 space) {
    vec4 p = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    p = space * p;
    return p.xyz / p.w;
}

vec3 reconstructPosition(vec2 uv, float depth) {
    return reconstructPosition(uv, depth, getViewProjectionInverse());
}
