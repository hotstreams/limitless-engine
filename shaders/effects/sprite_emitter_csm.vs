Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::EmitterType

#include "glsl/material.glsl"
#include "glsl/scene.glsl"

layout(location = 0) in vec3 position;
#if defined(InitialColor_MODULE)
    layout(location = 1) in vec4 color;
    out vec4 fs_color;
#endif
#if defined(InitialRotation_MODULE)
    layout(location = 2) in vec3 rotation;
    out vec3 fs_rotation;
#endif
#if defined(InitialVelocity_MODULE)
    layout(location = 3) in vec3 velocity;
    out vec3 fs_velocity;
#endif
#if defined(Lifetime_MODULE)
    layout(location = 4) in float lifetime;
    out float fs_lifetime;
#endif
#if defined(InitialSize_MODULE)
    layout(location = 5) in vec3 size;
    out vec3 fs_size;
#endif
#if defined(SubUV_MODULE)
    layout(location = 6) in vec4 subUV;
    out vec4 fs_subUV;
#endif
#if defined(CustomMaterial_MODULE)
    layout(location = 7) in vec4 properties;
    out vec4 fs_properties;
#endif

uniform mat4 light_space;

void main() {
	gl_Position = light_space * vec4(position, 1.0);

	#if defined(InitialSize_MODULE)
	    gl_PointSize = light_space[1][1] * size.x / gl_Position.w;
	#else
	    gl_PointSize = light_space[1][1] * 16.0 / gl_Position.w;
	#endif

    #if defined(InitialColor_MODULE)
        fs_color = color;
    #endif

    #if defined(InitialRotation_MODULE)
        fs_rotation = rotation;
    #endif

    #if defined(InitialVelocity_MODULE)
        fs_velocity = velocity;
    #endif

    #if defined(Lifetime_MODULE)
        fs_lifetime = lifetime;
    #endif

    #if defined(InitialSize_MODULE)
        fs_size = size;
    #endif

    #if defined(SubUV_MODULE)
        fs_subUV = subUV;
    #endif

    #if defined(CustomMaterial_MODULE)
        fs_properties = properties;
    #endif
}
