ENGINE::COMMON

#include "../../pipeline/scene.glsl"
#include "../../functions/random.glsl"

in vec2 uv;

uniform sampler2D ssao;

uniform float kernel[16];
uniform vec2 axis;
uniform uint sample_count;
uniform float far_plane_over_edge_distance;

out vec3 color;

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
    vec3 data = textureLod(ssao, position, 0.0).rgb;

    // bilateral sample
    float bilateral = weight * bilateralWeight(depth, unpack(data.gb));
    sum += data.r * bilateral;
    totalWeight += bilateral;
}

void main() {
    vec3 data = textureLod(ssao, uv, 0.0).rgb;
    // Filament bilateral blur keeps packed linear depth from the center texel only (not filtered in GB).
    vec2 center_gb = data.gb;

    // This is the skybox, skip
    if (data.g * data.b == 1.0) {
        color = data;
        return;
    }

    float depth = unpack(data.gb);
    float totalWeight = kernel[0];
    float sum = data.r * totalWeight;

    // Filament: axis is integer texel step (1,0)/(0,1); divide by *this* pass texture size, not full frame.
    // Using getResolution() here broke half-res AO: taps moved half a texel per step → ~2× weaker blur vs Filament.
    vec2 texel_size = axis / vec2(textureSize(ssao, 0));

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
    ao += ((getRandom(gl_FragCoord.xy - vec2(0.5)) - 0.5) / 255.0);

    color = vec3(ao, center_gb);
}