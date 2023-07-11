Limitless::GLSL_VERSION
Limitless::Extensions

in vec2 uv;

#include "../pipeline/scene.glsl"
#include "../functions/reconstruct_position.glsl"

layout (std430) readonly buffer SSAO_BUFFER {
    uint NOISE_SIZE;
    uint KERNEL_SAMPLES_COUNT;

    float RADIUS;
    float BIAS;

    vec4 samples[];
};

uniform sampler2D depth_texture;
uniform sampler2D normal_texture;
uniform sampler2D noise;

vec2 rotate(vec2 D, vec2 cos_sin) {
    return vec2(D.x * cos_sin.x - D.y * cos_sin.y, D.x * cos_sin.y + D.y * cos_sin.x);
}

vec3 getViewPosition(vec2 uv) {
    return mat3(getView()) * reconstructPosition(uv, texture(depth_texture, uv).r);
}

out float color;

void main() {
    float depth = texture(depth_texture, uv).r;
    if (abs(depth - 1.0f) < 0.00001) {
    	color = 1.0f;
        return;
    }

    const vec2 noise_scale = getResolution() / NOISE_SIZE;

    vec3 P = reconstructPosition(uv, depth);
    vec3 N = texture(normal_texture, uv).xyz;

    vec3 random = normalize(texture(noise, uv * noise_scale).xyz);

    vec3 tangent = normalize(random - N * dot(random, N));
    vec3 bitangent = cross(N, tangent);
    mat3 TBN = mat3(tangent, bitangent, N);


    float occlusion = 0.0;
    uint sample_size = 0u;
    for (uint i = 0u; i < KERNEL_SAMPLES_COUNT; ++i) {
        vec3 sample_pos = TBN * vec3(samples[i]);
        sample_pos = P + sample_pos * RADIUS;

        vec4 offset = vec4(sample_pos, 1.0);
        offset = getViewProjection() * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        float sample_d = texture(depth_texture, offset.xy).r;
        if (abs(sample_d - 1.0f) < 0.00001) {
            continue;
        }

        float sample_depth = reconstructPosition(uv, sample_d).z;

        float range_check = smoothstep(0.0, 1.0, RADIUS / abs(P.z - sample_depth));
        occlusion += (sample_depth >= sample_pos.z + BIAS ? 1.0 : 0.0) * range_check;

        ++sample_size;
    }

    occlusion = 1.0 - (occlusion / sample_size);

    color = occlusion;
}
