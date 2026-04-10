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
    mat4 _view_to_screen;
    mat4 _view;
    mat4 _view_inverse;
    mat4 _VP;
    mat4 _VP_inverse;
    vec4 _camera_position;
    vec2 _resolution;
    float _far_plane;
    float _near_plane;

    // Wind (global scene wind)
    vec4 _wind_dir_strength; // xyz dir, w strength
    vec4 _wind_params;       // x time, y speed, z scale, w quality (float)

    // SpeedTree wind constants (matches UE4 FSpeedTreeData layout)
    vec4 _st_wind_vector;
    vec4 _st_wind_global;
    vec4 _st_wind_branch;
    vec4 _st_wind_branch_twitch;
    vec4 _st_wind_branch_whip;
    vec4 _st_wind_branch_anchor;
    vec4 _st_wind_branch_adherences;
    vec4 _st_wind_turbulences;
    vec4 _st_wind_leaf1_ripple;
    vec4 _st_wind_leaf1_tumble;
    vec4 _st_wind_leaf1_twitch;
    vec4 _st_wind_leaf2_ripple;
    vec4 _st_wind_leaf2_tumble;
    vec4 _st_wind_leaf2_twitch;
    vec4 _st_wind_frond_ripple;
    // Optional rolling wind fields present in UE4; currently zeroed in Limitless.
    vec4 _st_wind_rolling_branch;
    vec4 _st_wind_rolling_leaf_and_direction;
    vec4 _st_wind_rolling_noise;
    vec4 _st_wind_animation;
    // Debug/control flags for Limitless (not part of UE4). Kept in a uvec4 to avoid float re-interpretation.
    uvec4 _st_wind_debug; // x = debug mask bits
};

mat4 getProjection() {
    return _projection;
}

mat4 getProjectionInverse() {
    return _projection_inverse;
}

mat4 getViewToScreen() {
    return _view_to_screen;
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

vec3 getWindDirection() {
    return _wind_dir_strength.xyz;
}

float getWindStrength() {
    return _wind_dir_strength.w;
}

float getWindTime() {
    return _wind_params.x;
}

float getWindSpeed() {
    return _wind_params.y;
}

float getWindScale() {
    return _wind_params.z;
}

float getWindQuality() {
    return _wind_params.w;
}

vec4 getSTWindVector() { return _st_wind_vector; }
vec4 getSTWindGlobal() { return _st_wind_global; }
vec4 getSTWindBranch() { return _st_wind_branch; }
vec4 getSTWindBranchTwitch() { return _st_wind_branch_twitch; }
vec4 getSTWindBranchWhip() { return _st_wind_branch_whip; }
vec4 getSTWindBranchAnchor() { return _st_wind_branch_anchor; }
vec4 getSTWindBranchAdherences() { return _st_wind_branch_adherences; }
vec4 getSTWindTurbulences() { return _st_wind_turbulences; }
vec4 getSTWindLeaf1Ripple() { return _st_wind_leaf1_ripple; }
vec4 getSTWindLeaf1Tumble() { return _st_wind_leaf1_tumble; }
vec4 getSTWindLeaf1Twitch() { return _st_wind_leaf1_twitch; }
vec4 getSTWindLeaf2Ripple() { return _st_wind_leaf2_ripple; }
vec4 getSTWindLeaf2Tumble() { return _st_wind_leaf2_tumble; }
vec4 getSTWindLeaf2Twitch() { return _st_wind_leaf2_twitch; }
vec4 getSTWindFrondRipple() { return _st_wind_frond_ripple; }
vec4 getSTWindRollingBranch() { return _st_wind_rolling_branch; }
vec4 getSTWindRollingLeafAndDirection() { return _st_wind_rolling_leaf_and_direction; }
vec4 getSTWindRollingNoise() { return _st_wind_rolling_noise; }
vec4 getSTWindAnimation() { return _st_wind_animation; }
uint getSTWindMask() { return _st_wind_debug.x; }
