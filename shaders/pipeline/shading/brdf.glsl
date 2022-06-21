// "An Inexpensive BRDF Model for Physically-Based Rendering", Schlick 1994
vec3 F_Schlick(const vec3 F0, float F90, float VoH) {
    return F0 + (F90 - F0) * pow5(1.0 - VoH);
}

vec3 F_Schlick(const vec3 F0, float VoH) {
    return F0 + (vec3(1.0) - F0) * pow5(1.0 - VoH);
}

float F_Schlick(float F0, float f90, float VoH) {
    return F0 + (f90 - F0) * pow5(1.0 - VoH);
}

float Fd_Lambert() {
    return 1.0 / PI;
}

// "Microfacet Models for Refraction through Rough Surfaces", Walter et al. 2007
float D_GGX(float roughness, float NoH, const vec3 h) {
    float oneMinusNoHSquared = 1.0 - NoH * NoH;
    float a = NoH * roughness;
    float k = roughness / (oneMinusNoHSquared + a * a);
    float d = k * k * (1.0 / PI);
    return d;
}

// "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs", Heitz 2014
float V_SmithGGXCorrelated(float roughness, float NoV, float NoL, float a2) {
    float lambdaV = NoL * sqrt((NoV - a2 * NoV) * NoV + a2);
    float lambdaL = NoV * sqrt((NoL - a2 * NoL) * NoL + a2);
    return 0.5 / (lambdaV + lambdaL);
}

vec3 SpecularF(const vec3 F0, float LoH) {
    float F90 = saturate(dot(F0, vec3(50.0 * 0.33)));
    return F_Schlick(F0, F90, LoH);
}

vec3 SpecularBRDF(float roughness, float NoH, float NoV, float NoL, float LoH, const vec3 H, const vec3 F0, float a2) {
    float D = D_GGX(roughness, NoH, H);
    float V = V_SmithGGXCorrelated(roughness, NoV, NoL, a2);
    vec3 F = SpecularF(F0, LoH);
    return (D * V) * F;
}

// "Physically-Based Shading at Disney", Burley 2012
float Fd_Burley(float roughness, float NoV, float NoL, float LoH) {
    float F90 = 0.5 + 2.0 * roughness * LoH * LoH;
    float lightScatter = F_Schlick(1.0, F90, NoL);
    float viewScatter  = F_Schlick(1.0, F90, NoV);
    return lightScatter * viewScatter * (1.0 / PI);
}

vec3 DiffuseBRDF(vec3 diffuse, float roughness, float NoV, float NoL, float LoH) {
    return diffuse * Fd_Burley(roughness, NoV, NoL, LoH);
}
