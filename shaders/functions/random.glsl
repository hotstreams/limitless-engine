/*
 *  gets random float value using interleaved gradient noise
 *
 *  frag_coord in range [0, resolution]
 *  uv in range [0, 1]
 *
 *  you can use gl_FragCoord.xy or uv * resolution
 */
float getRandom(vec2 frag_coord) {
    const vec3 m = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(m.z * fract(dot(frag_coord, m.xy)));
}