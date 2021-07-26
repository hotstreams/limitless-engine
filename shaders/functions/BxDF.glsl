#define PI 3.14159265359

vec3 Diffuse_Lambert(vec3 albedo) {
    return albedo * (1.0 / PI);
}

float SpecularD_NDF_GGX(float NoH, float a2) {
    float denom = (NoH * a2 - NoH) * NoH + 1.0;
    return a2 / (PI * denom * denom);
}

float SpecularG_Schlick(float cosTheta, float k) {
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

float SpecularG_Schlick(float NoV, float NoL, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return SpecularG_Schlick(NoV, k) * SpecularG_Schlick(NoL, k);
}

vec3 SpecularF_Schlick(vec3 F0, float cosTheta) {
    return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 Specular_BRDF(float NoV, float NoL, float NoH, float a2, float roughness, vec3 F) {
    float D = SpecularD_NDF_GGX(NoH, a2);
    float G = SpecularG_Schlick(NoV, NoL, roughness);
    return (D * F * G) / max((4 * NoV * NoL), 1e-3);
}

vec3 Diffuse_BRDF(vec3 albedo, vec3 F, float metallic) {
    vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic);
    return kd * Diffuse_Lambert(albedo);
}
