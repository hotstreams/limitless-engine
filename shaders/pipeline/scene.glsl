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
    mat4 _projection;
    mat4 _projection_inverse;
    mat4 _view;
    mat4 _view_inverse;
    mat4 _VP;
    mat4 _VP_inverse;
    vec4 _camera_position;
    vec2 _resolution;
    float _far_plane;
    float _near_plane;
};

mat4 getProjection() {
    return _projection;
}

mat4 getProjectionInverse() {
    return _projection_inverse;
}

mat4 getView() {
    return _view;
}

mat4 getViewInverse() {
    return _view_inverse;
}

mat4 getViewProjection() {
    return _VP;
}

mat4 getViewProjectionInverse() {
    return _VP_inverse;
}

vec3 getCameraPosition() {
    return _camera_position.xyz;
}

float getCameraFarPlane() {
    return _far_plane;
}

float getCameraNearPlane() {
    return _near_plane;
}

vec2 getResolution() {
    return _resolution;
}
