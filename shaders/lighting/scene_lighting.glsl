#include "./light.glsl"

layout (std140) buffer scene_lighting {
    vec4 _direction;
    vec4 _color;
    vec4 _ambient_color;
    uint _light_count;
};

Light getDirectionalLight() {
    Light light;

    // IMPORTANT: fully initialize the Light struct.
    // Uninitialized fields (especially `type`) lead to undefined behavior and can
    // make the directional light be treated as a punctual light with garbage data.
    light.color = _color;
    light.position = vec4(0.0);          // unused for directional
    light.direction = _direction;        // direction of light rays (world-space)
    light.scale_offset = vec2(0.0);      // unused for directional
    light.falloff = 0.0;                // unused for directional
    light.type = LIGHT_TYPE_DIRECTIONAL;

    return light;
}

vec4 getAmbientColor() {
    return _ambient_color;
}

uint getLightCount() {
    return _light_count;
}
