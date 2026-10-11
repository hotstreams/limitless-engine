#include "./scene_lighting.glsl"

vec3 computeAmbientLighting(vec3 albedo, float ao) {
    vec4 ambient = getAmbientColor();
    return (ambient.rgb * albedo) * (ambient.a * ao);
}

// Karis 2014, "Physically Based Shading on Mobile": an analytic fit of the split-sum environment BRDF, i.e. how much
// of a uniform environment a GGX surface reflects towards the viewer, as F0 * x + y.
vec2 environmentBRDFApprox(float perceptualRoughness, float NoV) {
    const vec4 c0 = vec4(-1.0, -0.0275, -0.572, 0.022);
    const vec4 c1 = vec4(1.0, 0.0425, 1.04, -0.04);
    vec4 r = perceptualRoughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28 * NoV)) * r.x + r.y;
    return vec2(-1.04, 1.04) * a004 + r.zw;
}

// The ambient light reflected specularly, as from a uniform environment of the ambient colour. Metals have no diffuse
// colour, so without it they get no ambient light at all; with it they show their colour in the shade, like under
// Blender's world lighting.
vec3 computeAmbientSpecular(vec3 F0, float perceptualRoughness, float NoV, float ao) {
    vec4 ambient = getAmbientColor();
    vec2 AB = environmentBRDFApprox(perceptualRoughness, NoV);
    return (ambient.rgb * (F0 * AB.x + AB.y)) * (ambient.a * ao);
}
