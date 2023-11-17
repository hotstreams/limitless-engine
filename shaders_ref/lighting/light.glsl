#define LIGHT_TYPE_POINT 0
#define LIGHT_TYPE_SPOT 1

struct Light {
    // rgb - linear color, a - intensity
    vec4 color;
    // world position
    vec3 position;
    // maximum radius
    float radius;
    // spot light attenuation
    vec2 scale_offset;
    // light type
    uint type;
    // does light cast shadow
    bool casts_shadow;
};

/*
    if opengl's SSBO extension is supported, all lights stored in SSBO

    if there is no SSBO, then engine defines MAX_LIGHTS
    which is guaranteed to be less or equal then UNIFORM MAX SIZE

    so actually there is limit on lights which is bound by uniform/ssbo max size divided by sizeof(light)
*/

#if defined (ENGINE_EXT_SSBO)
    layout (std140) buffer LIGHTS_BUFFER {
        Light _lights[];
    };
#else
    layout (std140) uniform LIGHTS_BUFFER {
        Light _lights[MAX_LIGHTS];
    };
#endif

Light getLight(const uint index) {
    return _lights[index];
}

vec3 computeLights(const ShadingContext sctx) {
    vec3 color = vec3(0.0);
    for (uint i = 0u; i < getLightsCount(); ++i) {
        Light light = getLight(i);

        LightingContext lctx = computeLightingContext(sctx, light);

        if (lctx.len > light.radius || lctx.NoL <= 0.0 || light.attenuation <= 0.0) {
            continue;
        }

#if defined (SHADOWS)
        if (light.casts_shadow) {

        }
#endif

        if (lctx.visibility <= 0.0) {
            continue;
        }

        color += shadeForLight(sctx, lctx, light);
    }

    return color;
}
