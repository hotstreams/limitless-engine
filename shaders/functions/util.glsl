#if defined(GL_ARB_derivative_control)
    #extension GL_ARB_gpu_shader5 : enable
#else
    #define fma(a, b, c) ((a) * (b) + (c))
#endif

#if defined(GL_ARB_derivative_control)
    #extension GL_ARB_derivative_control : enable
#else
    #define dFdxCoarse(a) dFdx(a)
    #define dFdyCoarse(a) dFdy(a)
#endif

float random(in vec2 xy) {
    return fract(sin(dot(xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 rotate_vec2(const vec2 v, const vec2 cs) {
    return vec2(cs.x * v.x + cs.y * v.y, cs.x * v.y - cs.y * v.x);
}