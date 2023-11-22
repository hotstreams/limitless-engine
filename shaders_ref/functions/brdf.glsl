
#if defined (ENGINE_MATERIAL_QUALITY_HIGH)
    #define ENGINE_MATERIAL_DIFFUSE_BURLEY
    #define ENGINE_MATERIAL_SPECULAR_GGX
    #define ENGINE_MATERIAL_SPECULAR_SCHLICK
#else
    #define ENGINE_MATERIAL_DIFFUSE_LAMBERT
    #define ENGINE_MATERIAL_SPECULAR_GGX_FAST
    #define ENGINE_MATERIAL_SPECULAR_SCHLICK_FAST
#endif

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

// "Microfacet Models for Refraction through Rough Surfaces", Walter et al. 2007
float D_GGX(float roughness, float NoH) {
    float oneMinusNoHSquared = 1.0 - NoH * NoH;
    float a = NoH * roughness;
    float k = roughness / (oneMinusNoHSquared + a * a);
    float d = k * k * (1.0 / PI);
    return d;
}

// "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs", Heitz 2014
float V_SmithGGXCorrelated(float NoV, float NoL, float a2, float lambdaV) {
    float lambdaL = NoV * sqrt((NoL - a2 * NoL) * NoL + a2);
    return 0.5 / (lambdaV + lambdaL);
}

// Hammon 2017, "PBR Diffuse Lighting for GGX+Smith Microsurfaces"
float V_SmithGGXCorrelatedFast(float roughness, float NoV, float NoL) {
    float v = 0.5 / mix(2.0 * NoL * NoV, NoL + NoV, roughness);
    return v;
}

vec3 SpecularF(const vec3 F0, float LoH) {
#if defined (ENGINE_MATERIAL_SPECULAR_SCHLICK)
    float F90 = saturate(dot(F0, vec3(50.0 * 0.33)));
    return F_Schlick(F0, F90, LoH);
#elif defined (ENGINE_MATERIAL_SPECULAR_SCHLICK_FAST)
    return F_Schlick(F0, LoH);
#endif
}

// Estevez and Kulla 2017, "Production Friendly Microfacet Sheen BRDF"
float D_Charlie(float roughness, float NoH) {
    float invAlpha  = 1.0 / roughness;
    float cos2h = NoH * NoH;
    float sin2h = max(1.0 - cos2h, 0.0078125); // 2^(-14/2), so sin2h^2 > 0 in fp16
    return (2.0 + invAlpha) * pow(sin2h, invAlpha * 0.5) / (2.0 * PI);
}

// Neubelt and Pettineo 2013, "Crafting a Next-gen Material Pipeline for The Order: 1886"
float V_Neubelt(float NoV, float NoL) {
    return 1.0 / (4.0 * (NoL + NoV - NoL * NoV));
}

// "Physically-Based Shading at Disney", Burley 2012
float Fd_Burley(float roughness, float NoV, float NoL, float LoH) {
    float F90 = 0.5 + 2.0 * roughness * LoH * LoH;
    float lightScatter = F_Schlick(1.0, F90, NoL);
    float viewScatter  = F_Schlick(1.0, F90, NoV);
    return lightScatter * viewScatter * (1.0 / PI);
}

float Fd_Lambert() {
    return 1.0 / PI;
}

// Energy conserving wrap diffuse term, does *not* include the divide by pi
float Fd_Wrap(float NoL, float w) {
    float w1 = 1.0 + w;
    float x = w1 * w1;
    return (NoL + w) / x;
}

vec3 DiffuseBRDF(const ShadingContext sctx, const LightingContext lctx) {
#if defined (ENGINE_MATERIAL_DIFFUSE_BURLEY)
    return sctx.diffuseColor * Fd_Burley(sctx.roughness, sctx.NoV, lctx.NoL, lctx.LoH);
#elif defined (ENGINE_MATERIAL_DIFFUSE_LAMBERT)
    return sctx.diffuseColor * Fd_Lambert();
#endif
}

vec3 SpecularBRDF(const ShadingContext sctx, const LightingContext lctx) {
    float D = D_GGX(sctx.roughness, lctx.NoH);
#if defined (ENGINE_MATERIAL_SPECULAR_GGX)
    float V = V_SmithGGXCorrelated(sctx.NoV, lctx.NoL, sctx.a2, lctxlambdaV);
#elif defined (ENGINE_MATERIAL_SPECULAR_GGX_FAST)
    float V = V_SmithGGXCorrelatedFast(sctx.roughness, sctx.NoV, lctx.NoL);
#endif
    vec3 F = SpecularF(sctx.F0, lctx.LoH);
    return (D * V) * F;
}

vec3 DiffuseBRDF_Cloth(const ShadingContext sctx, const LightingContext lctx) {
    vec3 diffuse = DiffuseBRDF(sctx, lctx);
#if defined (ENGINE_MATERIAL_SUBSURFACE_COLOR)
    diffuse *= Fd_Wrap(lsctx.NoL, 0.5);
#endif
    return diffuse;
}

vec3 SpecularBRDF_Cloth(const ShadingContext sctx, const LightingContext lctx) {
    float D = D_Charlie(sctx.roughness, lctx.NoH);
    float V = V_Neubelt(sctx.NoV, lctx.NoL);
    return (D * V) * sctx.F0;
}
