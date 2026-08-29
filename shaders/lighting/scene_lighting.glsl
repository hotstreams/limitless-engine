#include "./light.glsl"

layout (std140) buffer scene_lighting {
    vec4 sun_direction;
    vec4 sun_color;
    vec4 ambient_color;
    uint light_count;
};

Light getDirectionalLight() {
    Light light;

    light.direction.xyz = sun_direction.xyz;
    light.color = sun_color;

    return light;
}

vec4 getAmbientColor() {
    return ambient_color;
}

uint getLightCount() {
    return light_count;
}
