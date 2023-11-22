#include "./scene_lighting.glsl"

vec3 computeAmbientLighting(vec3 albedo, float ao) {
    vec4 ambient = getAmbientColor();
    return (ambient.rgb * albedo) * (ambient.a * ao);
}