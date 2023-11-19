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
