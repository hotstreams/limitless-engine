#define PI 3.14159265359
#define MIN_ROUGHNESS 0.002025
#define MIN_PERCEPTUAL_ROUGHNESS 0.045

#define MIN_NoV 1e-4

float pow5(float x) {
    float x2 = x * x;
    return x2 * x2 * x;
}

float saturate(float val) {
    return clamp(val, 0.0, 1.0);
}

vec2 saturate(vec2 val) {
    return clamp(val, 0.0, 1.0);
}

vec3 saturate(vec3 val) {
    return clamp(val, 0.0, 1.0);
}

vec4 saturate(vec4 val) {
    return clamp(val, 0.0, 1.0);
}

vec3 computeDiffuseColor(const vec3 baseColor, float metallic) {
    return baseColor * (1.0 - metallic);
}

vec3 computeF0(const vec3 baseColor, float metallic, float reflectance) {
    return baseColor * metallic + (reflectance * (1.0 - metallic));
}

float computeNoV(const vec3 N, const vec3 V) {
    float NoV = saturate(dot(N, V));
    return max(MIN_NoV, NoV);
}

// "Material Advances in Call of Duty: WWII", Chan 2018
float computeMicroShadowing(float NoL, float visibility) {
    float aperture = inversesqrt(1.0 - visibility);
    float microShadow = saturate(NoL * aperture);
    return microShadow * microShadow;
}

float specularAA(const vec3 normal, float perceptualRoughness, float aaThreshold, float aaVariance) {
    vec3 du = dFdx(normal);
    vec3 dv = dFdy(normal);

    float variance = aaVariance * (dot(du, du) + dot(dv, dv));

    float roughness = perceptualRoughness * perceptualRoughness;
    float kernelRoughness = min(2.0 * variance, aaThreshold);
    float squareRoughness = saturate(roughness * roughness + kernelRoughness);

    return sqrt(sqrt(squareRoughness));
}

vec4 clipToScreenPos(vec4 pos) {
    vec4 o = pos * 0.5f;
    o.xy += o.w;
    o.zw = pos.zw;
    return o;
}

vec4 ndcToClipPos(vec3 ndc) {
	// map xy to -1,1
	vec4 clip_pos = vec4(ndc.xy * 2.0f - 1.0f, ndc.z, 1.0f);
    clip_pos.z = clip_pos.z * 2.0f - 1.0f;
    return clip_pos;
}

vec3 ndcToWorldPos(mat4 inverseVP, vec3 ndc) {
	vec4 clip_pos = ndcToClipPos(ndc);
	vec4 pos = inverseVP * clip_pos;
	pos.xyz /= pos.w;

	return pos.xyz;
}
