#include "./lighting_context.glsl"
#include "../pipeline/scene.glsl"

#include "../shading/regular.glsl"
#include "../shading/custom.glsl"

#include "./scene_lighting.glsl"
#include "./shadows.glsl"

// CPU tiled light culling buffers (built on CPU, consumed in shading)
// - TILE_LIGHT_GRID: per-tile (start,count) into TILE_LIGHT_INDICES
// - TILE_LIGHT_INDICES: flattened list of light indices (into LIGHTS_BUFFER / getLight())
//
// NOTE: these are SSBOs; std430 is used for tight packing.
layout (std430) buffer TILE_LIGHT_GRID {
    uvec2 _tile_light_grid[];
};

layout (std430) buffer TILE_LIGHT_INDICES {
    uint _tile_light_indices[];
};

#ifndef ENGINE_SETTINGS_LIGHT_TILE_SIZE
    #define ENGINE_SETTINGS_LIGHT_TILE_SIZE 16
#endif

const uint ENGINE_LIGHT_TILE_SIZE = uint(ENGINE_SETTINGS_LIGHT_TILE_SIZE);

uvec2 getTileCounts() {
    uvec2 res = uvec2(max(vec2(1.0), getResolution()));
    return uvec2(
        (res.x + ENGINE_LIGHT_TILE_SIZE - 1u) / ENGINE_LIGHT_TILE_SIZE,
        (res.y + ENGINE_LIGHT_TILE_SIZE - 1u) / ENGINE_LIGHT_TILE_SIZE
    );
}

uvec2 getTileStartCount(uvec2 tile) {
    uvec2 tc = getTileCounts();
    tile = min(tile, tc - 1u);
    uint idx = tile.x + tile.y * tc.x;
    return _tile_light_grid[idx];
}

uint getCurrentTileLightCount() {
    uvec2 px = uvec2(floor(gl_FragCoord.xy));
    uvec2 tile = px / ENGINE_LIGHT_TILE_SIZE;
    return getTileStartCount(tile).y;
}

vec3 computeLight(const ShadingContext sctx, const LightingContext lctx, const Light light) {
    /* [forward pipeline] */
#if defined (ENGINE_MATERIAL_SHADING_REGULAR_MODEL)
    return regularShading(sctx, lctx, light);
#elif defined (ENGINE_MATERIAL_SHADING_CUSTOM_MODEL)
    return customShading(sctx, lctx, light);
#else
    /* [deferred pipeline] */
    switch (sctx.shading_model) {
        case ENGINE_SHADING_LIT: // Lit
            return regularShading(sctx, lctx, light);
        default:
            // deferred pipeline does not support custom shading
            // and unlit paths calculated before
            return vec3(322.0, 0.0, 0.0);
    }
#endif
}

vec3 computeDirectionalLight(const ShadingContext sctx) {
    Light light = getDirectionalLight();
    LightingContext lctx = computeLightingContext(sctx, light);
    lctx.attenuation = 1.0;

    vec3 color = vec3(0.0);

    if (lctx.NoL <= 0.0) {
        return color;
    }

    #if defined (ENGINE_SETTINGS_CSM)
        float shadow = getDirectionalShadow(sctx.N, sctx.worldPos);
        lctx.visibility *= (1.0 - shadow);
    #endif

    #if defined (ENGINE_SETTINGS_MICRO_SHADOWING)
        lctx.visibility *= computeMicroShadowing(lctx.NoL, sctx.ambientOcclusion);
    #endif

    if (lctx.visibility <= 0.0) {
       return color;
    }

    return computeLight(sctx, lctx, light);
}

vec3 computeLights(const ShadingContext sctx) {
    vec3 color = computeDirectionalLight(sctx);

#if defined (ENGINE_SETTINGS_SSR)
    color += sctx.indirect_lighting;
#endif

    uvec2 px = uvec2(floor(gl_FragCoord.xy));
    uvec2 tile = px / ENGINE_LIGHT_TILE_SIZE;
    uvec2 sc = getTileStartCount(tile);

    for (uint j = 0u; j < sc.y; ++j) {
        uint i = _tile_light_indices[sc.x + j];
        Light light = getLight(i);

        LightingContext lctx = computeLightingContext(sctx, light);

        if (lctx.NoL <= 0.0 || lctx.attenuation <= 0.0) {
            continue;
        }

        color += computeLight(sctx, lctx, light);
    }

    return color;
}