/*
    Vertex shader attributes interface:

    vec4 getParticleColor();
    vec4 getParticleSubUV();
    vec4 getParticleProperties();

    vec3 getParticleAcceleration();
    vec3 getParticlePosition();
    vec3 getParticleRotation();
    vec3 getParticleVelocity();

    float getParticleTime();

    SpriteEmitter & BeamEmitter
        float getParticleSize();

    MeshEmitter
        vec3 getMeshPosition();
        vec3 getMeshNormal();
        vec3 getMeshTangent();
        vec2 getMeshUV();

        mat4 getParticleModel();
        vec3 getParticleSize();
*/

Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::EmitterType

#include "../glsl/material.glsl"
#include "../glsl/scene.glsl"

#if defined(SpriteEmitter)
    #include "./sprite_vertex_stream.glsl"
#endif

#if defined(BeamEmitter)
    #include "./beam_vertex_stream.glsl"
#endif

#if defined(MeshEmitter)
    #include "./mesh_vertex_stream.glsl"
#endif

out vertex_data {
    #if defined(InitialColor_MODULE)
        vec4 color;
    #endif

    #if defined(SubUV_MODULE)
        vec4 subUV;
    #endif

    #if defined(CustomMaterial_MODULE)
        vec4 properties;
    #endif

    #if defined(Lifetime_MODULE) || defined(Acceleration_MODULE)
        vec3 acceleration;
        float lifetime;
    #endif

    vec3 position;

    #if defined(SpriteEmitter)
        float size;
    #endif

    #if defined(InitialRotation_MODULE) || defined(Time_MODULE)
        vec3 rotation;
        flat float time;
    #endif

    #if (defined(SpriteEmitter) || defined(MeshEmitter)) && defined(InitialVelocity_MODULE)
        vec3 velocity;
    #endif

    #if defined(BeamEmitter) && (defined(InitialVelocity_MODULE) || defined(InitialSize_MODULE))
        vec3 velocity;
        float size;
    #endif

    #if defined(MeshEmitter)
        vec3 size;
        vec3 normal;
    #endif

    #if defined(BeamEmitter) || defined(MeshEmitter)
        vec2 uv;
    #endif

    #if defined(BeamEmitter) && defined(BeamSpeed_MODULE)
        vec3 start;
        vec3 end;
        float length;
    #endif
} out_data;

void main() {
    #if defined(SpriteEmitter)
        vec3 vertex_position = getParticlePosition();

        Limitless::CustomMaterialVertexCode

        out_data.position = vertex_position;
	    gl_Position = VP * vec4(out_data.position, 1.0);
	#endif

	#if defined(BeamEmitter)
	    vec2 uv = getParticleUV();
	    vec4 vertex_position = getParticlePosition();

        Limitless::CustomMaterialVertexCode

        out_data.position = vec3(vertex_position);
        gl_Position = VP * vertex_position;
        out_data.uv = uv;

        #if defined(BeamSpeed_MODULE)
            out_data.start = getParticleStart();
            out_data.end = getParticleEnd();
            out_data.length = getParticleLength();
        #endif
    #endif

    #if defined(MeshEmitter)
        vec2 uv = getMeshUV();
        vec3 vertex_position = getMeshPosition();

        Limitless::CustomMaterialVertexCode

        out_data.position = vec3(getParticleModel() * vec4(vertex_position, 1.0));
        gl_Position = VP * getParticleModel() * vec4(vertex_position, 1.0);
        out_data.uv = uv;

        out_data.normal = transpose(inverse(mat3(getParticleModel()))) * getMeshNormal();
    #endif

    #if defined(SpriteEmitter)
        #if defined(InitialSize_MODULE)
            gl_PointSize = projection[1][1] * getParticleSize() / gl_Position.w;
        #else
            gl_PointSize = projection[1][1] * 16.0 / gl_Position.w;
        #endif
    #endif

    #if defined(InitialColor_MODULE)
        out_data.color = getParticleColor();
    #endif

    #if defined(SubUV_MODULE)
        out_data.subUV = getParticleSubUV();
    #endif

    #if defined(CustomMaterial_MODULE)
        out_data.properties = getParticleProperties();
    #endif

    #if defined(InitialRotation_MODULE) || defined(Time_MODULE)
        out_data.rotation = getParticleRotation();
        out_data.time = getParticleTime();
    #endif

    #if defined(SpriteEmitter) && defined(InitialVelocity_MODULE)
        out_data.velocity = getParticleVelocity();
    #endif

    #if defined(BeamEmitter) && (defined(InitialVelocity_MODULE) || defined(InitialSize_Module))
        out_data.velocity = getParticleVelocity();
        out_data.size = getParticleSize();
    #endif

    #if defined(Lifetime_MODULE) || defined(Acceleration_MODULE)
        out_data.acceleration = getParticleAcceleration();
        out_data.lifetime = getParticleLifetime();
    #endif

    #if defined(SpriteEmitter)
        out_data.size = getParticleSize();
    #endif
}
