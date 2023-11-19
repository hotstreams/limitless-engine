#include "./lighting_context.glsl"
#include "../scene.glsl"
#include "../shading/shade_for_light.glsl"

vec3 computeLights(const ShadingContext sctx) {
    vec3 color = vec3(0.0);
    //TODO: lights count
    //for (uint i = 0u; i < getLightsCount(); ++i) {
    for (uint i = 0u; i < 0u; ++i) {
        Light light = getLight(i);

        LightingContext lctx = computeLightingContext(sctx, light);

        if (lctx.len > light.radius || lctx.NoL <= 0.0 || lctx.attenuation <= 0.0) {
            continue;
        }

        #if defined (SHADOWS)
        if (light.casts_shadow) {

        }
        #endif

        if (lctx.visibility <= 0.0) {
            continue;
        }

        //color += shadeForLight(sctx, lctx, light);
    }

    return color;
}