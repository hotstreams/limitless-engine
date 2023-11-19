#include "./linearize_depth.glsl"
#include "../scene.glsl"

vec3 reconstructPosition(vec2 uv, float depth) {
   vec4 p = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
   p = getViewProjectionInverse() * p;
   return p.xyz / p.w;
}

/**
 *  Reconstructs world position from depth

 *  uv - normalized coordinates [0, 1]
 *  depth - normalized depth [0, 1]
 *  fov_scale2 - {2.0 * tan(getProjectionInverse()[0][0] / 2.0),
 *                2.0 * tan(getProjectionInverse()[1][1] / 2.0)}
 */
vec3 reconstructViewSpacePosition(vec2 uv, float depth) {
    vec2 fov_scale2 = 2.0 * vec2(tan(getProjectionInverse()[0][0] / 2.0), tan(getProjectionInverse()[1][1] / 2.0));

    float z = linearize_depth(depth, getCameraNearPlane(), getCameraFarPlane());
    vec2 half_ndc = vec2(0.5) - uv; // multiplication * 2.0 done in fov_scale2
    return vec3(half_ndc * fov_scale2 * -z, -z);
}
