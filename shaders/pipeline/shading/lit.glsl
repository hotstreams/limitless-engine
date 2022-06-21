#include "./common.glsl"
#include "./brdf.glsl"

#include "../lighting/lighting_context.glsl"
#include "../lighting/scene_lighting.glsl"

#include "../lighting/ambient.glsl"
#include "../lighting/directional_light.glsl"
#include "../lighting/point_light.glsl"
#include "../lighting/spot_light.glsl"

vec3 litShading(const vec3 baseColor, float ao, const vec3 N, const vec3 P, float metallic, float roughness) {
    LightingContext context = computeLightingContext(baseColor, metallic, P, roughness, N, ao);

    vec3 color = getAmbientLighting(baseColor, ao);

    color += computeDirectionalLight(context);

    color += computePointLight(context);

    color += computeSpotLight(context);

    return color;
}