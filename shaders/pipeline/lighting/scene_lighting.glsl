struct DirectionalLight {
    vec4 direction;
    vec4 color;
};

layout (std140) buffer scene_lighting {
    DirectionalLight _dir_light;
    vec4 _ambient_color;
    uint _point_lights_count;
    uint _dir_lights_count;
    uint _spot_lights_count;
};

DirectionalLight getDirectionalLight() {
    return _dir_light;
}

vec4 getAmbientColor() {
    return _ambient_color;
}

uint getPointLightsCount() {
    return _point_lights_count;
}

uint getDirectionalLightsCount() {
    return _dir_lights_count;
}

uint getSpotLightsCount() {
    return _spot_lights_count;
}
