// dFdxCoarse / dFdyCoarse / fma: injected in the shader preamble by
// ContextInitializer::getGlslBuiltinFallbackDefines() and optional
// GL_ARB_derivative_control extension (see ShaderDefineReplacer::getExtensionDefine).
// Do not use #extension in this include file.

float random(in vec2 xy) {
    return fract(sin(dot(xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 rotate_vec2(const vec2 v, const vec2 cs) {
    return vec2(cs.x * v.x + cs.y * v.y, cs.x * v.y - cs.y * v.x);
}
