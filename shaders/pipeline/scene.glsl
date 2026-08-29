/*
    mat4 getProjection();
    mat4 getProjectionInverse();
    mat4 getView();
    mat4 getViewInverse();
    mat4 getViewProjection();
    mat4 getViewProjectionInverse();
    vec3 getCameraPosition();
*/

layout (std140) uniform scene_data {
    mat4 projection;
    mat4 projection_inverse;
    mat4 view_to_screen;
    mat4 view;
    mat4 view_inverse;
    mat4 VP;
    mat4 VP_inverse;
    vec4 camera_position;
    vec2 resolution;
    float far_plane;
    float near_plane;
};

mat4 getProjection() {
    return projection;
}

mat4 getProjectionInverse() {
    return projection_inverse;
}

mat4 getViewToScreen() {
    return view_to_screen;
}

mat4 getView() {
    return view;
}

mat4 getViewInverse() {
    return view_inverse;
}

mat4 getViewProjection() {
    return VP;
}

mat4 getViewProjectionInverse() {
    return VP_inverse;
}

vec3 getCameraPosition() {
    return camera_position.xyz;
}

float getCameraFarPlane() {
    return far_plane;
}

float getCameraNearPlane() {
    return near_plane;
}

vec2 getResolution() {
    return resolution;
}
