#include "../../functions/BxDF.glsl"
#include "../material/shading.glsl"

#include "./scene_lighting.glsl"
#include "./lighting_context.glsl"
#include "./directional_light.glsl"
#include "./point_light.glsl"
#include "./spot_light.glsl"
#include "./ssao.glsl"

vec3 getAmbientLighting(vec3 albedo, float ao, vec2 uv) {
    return (getAmbientColor().rgb * albedo) * (getAmbientColor().a * ao * getSSAO(uv));
}

vec3 getUnlitShadingColor(vec3 albedo, float ao, vec2 uv) {
    return getAmbientLighting(albedo, ao, uv);
}

vec3 getLitShadingColor(vec3 albedo, float ao, vec3 N, vec3 P, vec2 uv, float metallic, float roughness) {
    lighting_context context = init_lighting_context(N, albedo.rgb, P, metallic, roughness);

    vec3 light = vec3(0.0);

    /* direct global lighting */
    light += getGlobalLightDirectLighting(context);

    /* direct point lighting */
    light += getPointLightDirectLighting(context);

    /* direct spot lighting */
    light += getSpotLightDirectLighting(context);

    vec3 ambient = getAmbientLighting(albedo, ao, uv);

    return ambient + light;
}

vec3 getFragmentColor(vec3 albedo, float ao, vec3 N, vec3 P, vec2 uv, float metallic, float roughness, uint shading_model) {
    switch (shading_model) {
        case LIT_SHADING:
            return getLitShadingColor(albedo, ao, N, P, uv, metallic, roughness);
        case UNLIT_SHADING:
            return getUnlitShadingColor(albedo, ao, uv);
        default:
            return vec3(0.0);
    }
}
