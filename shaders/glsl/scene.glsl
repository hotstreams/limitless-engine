layout(std140) uniform scene_data {
    mat4 projection;
    mat4 view;
    mat4 VP;
    vec4 camera_position;
    vec4 ambient_color;
    uint point_lights_count;
};