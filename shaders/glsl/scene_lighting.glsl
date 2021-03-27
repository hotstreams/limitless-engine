struct DirLight {
    vec4 direction;
    vec4 color;
};

layout(std140) buffer scene_lighting {
    DirLight dir_light;
    vec4 ambient_color;
    uint point_lights_count;
    uint dir_lights_count;
};