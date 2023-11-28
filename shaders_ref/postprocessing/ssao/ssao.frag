ENGINE::COMMON

in vec2 uv;

#include "../../pipeline/scene.glsl"
#include "../../functions/reconstruct_position.glsl"
#include "../../functions/reconstruct_normal.glsl"
#include "../../functions/random.glsl"
#include "../../functions/common.glsl"

layout (std140) uniform SSAO_BUFFER {
    vec2 sample_count;
    vec2 angle_inc_cos_sin;
    float projection_scale_radius;
    float intensity;
    float spiral_turns;
    float inv_radius_squared;
    float min_horizon_angle_sine_squared;
    float bias;
    float peak2;
    float power;
    uint max_level;
};

uniform sampler2D depth_texture;

out vec3 color;

vec3 tapLocation(float i, const float noise) {
    float offset = ((2.0 * PI) * 2.4) * noise;
    float angle = ((i * sample_count.y) * spiral_turns) * (2.0 * PI) + offset;
    float radius = (i + noise + 0.5) * sample_count.y;
    return vec3(cos(angle), sin(angle), radius * radius);
}

highp vec2 startPosition(const float noise) {
    float angle = ((2.0 * PI) * 2.4) * noise;
    return vec2(cos(angle), sin(angle));
}

highp mat2 tapAngleStep() {
    vec2 t = angle_inc_cos_sin;
    return mat2(t.x, t.y, -t.y, t.x);
}

vec3 tapLocationFast(float i, vec2 p, const float noise) {
    float radius = (i + noise + 0.5) * sample_count.y;
    return vec3(p, radius * radius);
}

void computeAmbientOcclusionSAO(inout float occlusion, inout vec3 bentNormal,
                                float i, float ssDiskRadius,
                                const highp vec2 uv,  const highp vec3 origin, const vec3 normal,
                                const vec2 tapPosition, const float noise) {

    vec3 tap = tapLocationFast(i, tapPosition, noise);
    float ssRadius = max(1.0, tap.z * ssDiskRadius);

    vec2 uvSamplePos = uv + vec2(ssRadius * tap.xy) * 1.0 / getResolution();

    float level = clamp(floor(log2(ssRadius)) - 3.0, 0.0, float(max_level));
    //TODO: make mipmap depth
    float occlusionDepth = texture(depth_texture, uvSamplePos).r;

    vec3 p = reconstructViewSpacePosition(uvSamplePos, occlusionDepth);

    // now we have the sample, compute AO
    highp vec3 v = p - origin;  // sample vector
    float vv = dot(v, v);       // squared distance
    float vn = dot(v, normal);  // distance * cos(v, normal)

    // discard samples that are outside of the radius, preventing distant geometry to
    // cast shadows -- there are many functions that work and choosing one is an artistic
    // decision.
    float s = max(0.0, 1.0 - vv * inv_radius_squared);
    float w = s * s;

    // discard samples that are too close to the horizon to reduce shadows cast by geometry
    // not sufficently tessellated. The goal is to discard samples that form an angle 'beta'
    // smaller than 'epsilon' with the horizon. We already have dot(v,n) which is equal to the
    // sin(beta) * |v|. So the test simplifies to vn^2 < vv * sin(epsilon)^2.
    w *= step(vv * min_horizon_angle_sine_squared, vn * vn);

    float sampleOcclusion = max(0.0, vn + (origin.z * bias)) / (vv + peak2);
    occlusion += w * sampleOcclusion;
}

/*
 * https://research.nvidia.com/sites/default/files/pubs/2012-06_Scalable-Ambient-Obscurance/McGuire12SAO.pdf
 */
void scalableAmbientObscurance(out float obscurance, out vec3 bentNormal, vec2 uv, vec3 origin, vec3 normal) {
    float noise = getRandom(gl_FragCoord.xy);
    highp vec2 tapPosition = startPosition(noise);
    highp mat2 angleStep = tapAngleStep();

    // Choose the screen-space sample radius
    // proportional to the projected area of the sphere
    float ssDiskRadius = -(projection_scale_radius / origin.z);

    obscurance = 0.0;
    bentNormal = normal;
    for (float i = 0.0; i < sample_count.x; i += 1.0) {
        computeAmbientOcclusionSAO(obscurance, bentNormal, i, ssDiskRadius, uv, origin, normal, tapPosition, noise);
        tapPosition = angleStep * tapPosition;
    }
    obscurance = sqrt(obscurance * intensity);
}

vec2 pack(highp float normalizedDepth) {
    // we need 16-bits of precision
    highp float z = clamp(normalizedDepth, 0.0, 1.0);
    highp float t = floor(256.0 * z);
    mediump float hi = t * (1.0 / 256.0);   // we only need 8-bits of precision
    mediump float lo = (256.0 * z) - t;     // we only need 8-bits of precision
    return vec2(hi, lo);
}

highp float unpack(highp vec2 depth) {
    // depth here only has 8-bits of precision, but the unpacked depth is highp
    // this is equivalent to (x8 * 256 + y8) / 65535, which gives a value between 0 and 1
    return (depth.x * (256.0 / 257.0) + depth.y * (1.0 / 257.0));
}

void main() {
    float depth = texture(depth_texture, uv).r;
    float z = linearize_depth(depth, getCameraNearPlane(), getCameraFarPlane());

    vec3 position = reconstructViewSpacePosition(uv, depth);
    vec3 normal = reconstructViewSpaceNormal(depth_texture, uv, depth, position, vec2(1.0) / getResolution());

    float occlusion = 0.0;
    vec3 bentNormal; // will be discarded
    scalableAmbientObscurance(occlusion, bentNormal, uv, position, normal);

    float aoVisibility = pow(saturate(1.0 - occlusion), power);

    color = vec3(aoVisibility, pack(position.z * 1.0 / getCameraFarPlane()));
}
