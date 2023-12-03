#include "./light.glsl"

layout (std140) buffer scene_lighting {
    vec4 _direction;
    vec4 _color;
    vec4 _ambient_color;
    uint _light_count;
};

Light getDirectionalLight() {
    Light light;

    light.direction.xyz = _direction.xyz;
    light.color = _color;

    return light;
}

vec4 getAmbientColor() {
    return _ambient_color;
}

uint getLightCount() {
    return _light_count;
}
