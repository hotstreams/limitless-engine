ENGINE::COMMON

#include "../../pipeline/scene.glsl"
#include "../../functions/reconstruct_position.glsl"
#include "../../functions/random.glsl"
#include "../../functions/math.glsl"
#include "../../functions/trace_ray.glsl"
#include "../../functions/common.glsl"
#include "../../functions/brdf.glsl"

// input normalized uv
in vec2 uv;

// output vec3 reflected_color color
out vec3 color;

// GBUFFER textures
uniform sampler2D normal_texture;
uniform sampler2D depth_texture;
uniform sampler2D props_texture;
uniform sampler2D base_color_texture;

// SSR settings
uniform float vs_thickness = 0.5;
uniform float stride = 2.0;
uniform float vs_max_distance = 1000.0;
uniform float bias = 0.1;
uniform float max_steps = 1000.0;

uniform float reflection_threshold = 0.0;
uniform float roughness_factor = 0.1;

uniform float camera_attenuation_lower_edge = 0.2;
uniform float camera_attenuation_upper_edge = 0.55;

uniform float reflection_strength = 5.0;
uniform float reflection_falloff_exp = 1.0;

float compute_attenuation(ivec2 hitPixel, vec2 hitUV, vec3 vsRayOrigin, vec3 vsHitPoint, float maxRayDistance, float numIterations) {
    float attenuation = 1.0;

#ifdef ENGINE_SETTINGS_SSR_BORDERS_ATTENUATION
    // Attenuation against the border of the screen
    vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5) - hitUV.xy));

    attenuation *= clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
#endif

#ifdef ENGINE_SETTINGS_SSR_INTERSECTION_DISTANCE_ATTENUATION
    // Attenuation based on the distance between the origin of the reflection ray and the intersection point
    attenuation *= 1.0 - clamp(distance(vsRayOrigin, vsHitPoint) / maxRayDistance, 0.0, 1.0);
#endif

#ifdef ENGINE_SETTINGS_SSR_ITERATION_COUNT_ATTENUATION
    // Attenuation based on the number of iterations performed to find the intersection
    attenuation *= 1.0 - (numIterations / max_steps);
#endif

    return attenuation;
}

void main() {
    vec2 props = texture(props_texture, uv).rg;
    float roughness = props.r;
    float metallic = props.g;

    if (metallic < reflection_threshold) {
        return;
    }

    // normalized depth
    float depth = texture(depth_texture, uv).r;

    // skip if skybox
    if (abs(depth - 1.0) < 0.000001) {
        return;
    }

    // world space position
    vec3 ws_position = reconstructPosition(uv, depth);

    // world space view direction
    vec3 ws_view_dir = normalize(ws_position - getCameraPosition());

    // world space normal (already normalized)
    vec3 ws_normal = texture(normal_texture, uv).xyz;
    
    // world space reflected_color vector from current position
    vec3 ws_reflected = reflect(ws_view_dir, ws_normal);

    // world space ray direction
    vec3 ws_ray_direction = normalize(ws_reflected);
    
    // world space ray origin
    vec3 ws_origin = ws_position + bias * ws_ray_direction;

    // view space ray origin
    vec3 vs_origin = mul_mat4_vec3(getView(), ws_origin).xyz;
    
    // view space ray direction (independent of camera position)
    vec3 vs_ray_direction = mul_mat3_vec3(getView(), ws_ray_direction);

    float attenuation = 1.0;
#ifdef ENGINE_SETTINGS_SSR_CAMERA_FACING_ATTENUATION
//     This will check the direction of the reflection vector with the view direction,
//     and if they are pointing in the same direction, it will drown out those reflections
//     since we are limited to pixels visible on screen. Attenuate reflections for angles between
//     60 degrees and 75 degrees, and drop all contribution beyond the (-60,60)  degree range
    attenuation *= 1.0 - smoothstep(camera_attenuation_lower_edge, camera_attenuation_upper_edge, dot(-ws_view_dir, ws_reflected));
    if (attenuation <= 0) {
        return;
    }
#endif

    vec3 jitt = vec3(0.0);
#ifndef SCREEN_SPACE_REFLECTIONS_BLUR
    jitt = mix(vec3(0.0), hash(vs_origin) - vec3(0.5), roughness) * roughness_factor; // jittering of the reflection direction to simulate roughness
#endif

    vec2 uv2 = uv * getResolution();
    float c = (uv2.x + uv2.y) * 0.25;
    float jitter = mod(c, 1.0); // jittering to hide artefacts when stepSize is > 1

    // Outputs from the traceScreenSpaceRay function.
    vec2 hitPixel;  // not currently used
    vec3 hitPoint;
    float numIterations;
    
    vec3 reflected_color = vec3(0.0);
    if (traceScreenSpaceRay(
            vs_origin,
            normalize(vs_ray_direction + jitt),
            getViewToScreen(),
            depth_texture,
            getResolution(),
            vs_thickness,
            getCameraNearPlane(),
            stride,
            jitter,
            max_steps,
            vs_max_distance,
            0,
            hitPixel,
            hitPoint,
            numIterations))
    {
        vec3 base_color = texelFetch(base_color_texture, ivec2(hitPixel), 0).rgb;

        attenuation *= compute_attenuation(ivec2(hitPixel),
                                           hitPixel / getResolution(),
                                           vs_origin,
                                           hitPoint,
                                           vs_max_distance,
                                           numIterations);

#ifdef ENGINE_SETTINGS_SSR_FRESNEL_ATTENUATION
        vec3 F0 = computeF0(base_color, metallic, 1.0);
        vec3 scatter = F_Schlick(F0, 1.0, max(dot(ws_normal, -ws_view_dir), 0.0));

        vec3 reflection_attenuation = clamp(pow(scatter * reflection_strength, vec3(reflection_falloff_exp)), 0.0, 1.0);
        reflected_color = base_color * (1.0 - reflection_attenuation) + base_color * attenuation * reflection_attenuation;
#else
        float reflection_attenuation = clamp(pow(metallic * reflection_strength, reflection_falloff_exp), 0.0, 1.0);
        reflected_color = base_color * (1.0 - reflection_attenuation) + base_color * attenuation * reflection_attenuation;
#endif
    }

    color = reflected_color;
}