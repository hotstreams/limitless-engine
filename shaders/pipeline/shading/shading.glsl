#include "../material/shading.glsl"
#include "./lit.glsl"
#include "./unlit.glsl"

vec3 getFragmentColor(vec3 albedo, float ao, vec3 N, vec3 P, float metallic, float roughness, uint shading_model) {
    switch (shading_model) {
        case LIT_SHADING:
            return litShading(albedo, ao, N, P, metallic, roughness);
        case UNLIT_SHADING:
            return unlitShading(albedo, ao);
        default:
            return vec3(0.0);
    }
}