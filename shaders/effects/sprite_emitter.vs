Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::EmitterType

#include "../glsl/material.glsl"
#include "../glsl/scene.glsl"

layout(location = 0) in vec3 position;
#if defined(InitialColor_MODULE)
    layout(location = 1) in vec4 color;
#endif
#if defined(InitialRotation_MODULE)
    layout(location = 2) in vec3 rotation;
#endif
#if defined(InitialVelocity_MODULE)
    layout(location = 3) in vec3 velocity;
#endif
#if defined(Lifetime_MODULE)
    layout(location = 4) in float lifetime;
#endif
#if defined(InitialSize_MODULE)
    layout(location = 5) in vec3 size;
#endif
#if defined(SubUV_MODULE)
    layout(location = 6) in vec4 subUV;
#endif
#if defined(CustomMaterial_MODULE)
    layout(location = 7) in vec4 properties;
#endif

out vertex_data {
    #if defined(InitialColor_MODULE)
        vec4 color;
    #endif
    #if defined(InitialRotation_MODULE)
        vec3 rotation;
    #endif
    #if defined(InitialVelocity_MODULE)
        vec3 velocity;
    #endif
    #if defined(Lifetime_MODULE)
        float lifetime;
    #endif
    #if defined(InitialSize_MODULE)
        vec3 size;
    #endif
    #if defined(SubUV_MODULE)
        vec4 subUV;
    #endif
    #if defined(CustomMaterial_MODULE)
        vec4 properties;
    #endif
} out_data;

void main() {
	gl_Position = VP * vec4(position, 1.0);

	#if defined(InitialSize_MODULE)
        gl_PointSize = projection[1][1] * size.x / gl_Position.w;
    #else
        gl_PointSize = projection[1][1] * 16.0 / gl_Position.w;
    #endif

    #if defined(InitialColor_MODULE)
        out_data.color = color;
    #endif

    #if defined(InitialRotation_MODULE)
        out_data.rotation = rotation;
    #endif

    #if defined(InitialVelocity_MODULE)
        out_data.velocity = velocity;
    #endif

    #if defined(Lifetime_MODULE)
        out_data.lifetime = lifetime;
    #endif

    #if defined(InitialSize_MODULE)
        out_data.size = size;
    #endif

    #if defined(SubUV_MODULE)
        out_data.subUV = subUV;
    #endif

    #if defined(CustomMaterial_MODULE)
        out_data.properties = properties;
    #endif
}
