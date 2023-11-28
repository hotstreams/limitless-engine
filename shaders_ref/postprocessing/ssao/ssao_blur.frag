ENGINE::COMMON

#include "../../pipeline/scene.glsl"
#include "../../functions/random.glsl"

in vec2 uv;

uniform sampler2D ssao;

uniform float kernel[16];
uniform vec2 axis;
uniform uint sample_count;
uniform float far_plane_over_edge_distance;

out float color;

float unpack(vec2 depth) {
    // depth here only has 8-bits of precision, but the unpacked depth is highp
    // this is equivalent to (x8 * 256 + y8) / 65535, which gives a value between 0 and 1
    return (depth.x * (256.0 / 257.0) + depth.y * (1.0 / 257.0));
}

float bilateralWeight(in highp float depth, in highp float sampleDepth) {
    float diff = (sampleDepth - depth) * far_plane_over_edge_distance;
    return max(0.0, 1.0 - diff * diff);
}

void tap(inout float sum, inout float totalWeight, float weight, float depth, vec2 position) {
    // ambient occlusion sample
    vec3 data = texture(ssao, position).rgb;

    // bilateral sample
    float bilateral = weight * bilateralWeight(depth, unpack(data.gb));
    sum += data.r * bilateral;
    totalWeight += bilateral;
}

void main() {
    vec3 data = texture(ssao, uv).rgb;

    // This is the skybox, skip
    if (data.g * data.b == 1.0) {
        color= data.r;
        return;
    }

    float depth = unpack(data.gb);
    float totalWeight = kernel[0];
    float sum = data.r * totalWeight;

    vec2 texel_size = axis / getResolution();

    vec2 offset = texel_size;
    for (int i = 1; i < int(sample_count); i++) {
        float weight = kernel[i];
        tap(sum, totalWeight, weight, depth, uv + offset);
        tap(sum, totalWeight, weight, depth, uv - offset);
        offset += texel_size;
    }

    float ao = sum * (1.0 / totalWeight);

    // simple dithering helps a lot (assumes 8 bits target)
    // this is most useful with high quality/large blurs
    ao += ((getRandom(uv) - 0.5) / 255.0);

    color = ao;
}