Limitless::GLSL_VERSION
#extension GL_ARB_tessellation_shader : enable
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::ModelType
Limitless::EmitterType

layout(quads, equal_spacing, ccw) in;

#include "../pipeline/scene.glsl"
#include "../pipeline/material/material.glsl"

#include "../pipeline/interface_block/tess_control_input.glsl"
#include "../pipeline/interface_block/vertex.glsl"

float interpolate1D(float v0, float v1, float v2) {
    return gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2;
}

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2) {
    return gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2) {
    return gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2;
}

mat3 interpolateMat3(mat3 v0, mat3 v1, mat3 v2) {
    return gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2;
}

float interpolate1D(float v0, float v1, float v2, float v3) {
    float u = mix(v0, v1, gl_TessCoord.x);
    float v = mix(v2, v3, gl_TessCoord.x);
    return mix(u, v, gl_TessCoord.y);
}

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2, vec2 v3) {
    vec2 u = mix(v0, v1, gl_TessCoord.x);
    vec2 v = mix(v2, v3, gl_TessCoord.x);
    return mix(u, v, gl_TessCoord.y);
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2, vec3 v3) {
    vec3 u = mix(v0, v1, gl_TessCoord.x);
    vec3 v = mix(v2, v3, gl_TessCoord.x);
    return mix(u, v, gl_TessCoord.y);
}

mat3 interpolateMat3(mat3 v0, mat3 v1, mat3 v2, mat3 v3) {
    mat3 u;
    u[0] = mix(v0[0], v1[0], gl_TessCoord.x);
    u[1] = mix(v0[1], v1[1], gl_TessCoord.x);
    u[2] = mix(v0[2], v1[2], gl_TessCoord.x);

    mat3 v;
    v[0] = mix(v2[0], v3[0], gl_TessCoord.x);
    v[1] = mix(v2[1], v3[1], gl_TessCoord.x);
    v[2] = mix(v2[2], v3[2], gl_TessCoord.x);

    mat3 r;
    r[0] = mix(u[0], v[0], gl_TessCoord.y);
    r[1] = mix(u[1], v[1], gl_TessCoord.y);
    r[2] = mix(u[2], v[2], gl_TessCoord.y);

    return r;
}

void main() {
    #if defined (EFFECT_MODEL)
            _out_data.world_position = interpolate3D(getVertexPosition(0u), getVertexPosition(1u), getVertexPosition(2u), getVertexPosition(3u));

        #if !defined (SpriteEmitter)
            _out_data.uv = interpolate2D(getVertexUV(0u), getVertexUV(1u), getVertexUV(2u), getVertexUV(3u));
        #endif

        #if defined (BeamEmitter) && defined (BeamSpeed_MODULE)
            _out_data.start = interpolate3D(getParticleStart(0u), getParticleStart(1u), getParticleStart(2u), getParticleStart(3u));
            _out_data.end = interpolate3D(getParticleEnd(0u), getParticleEnd(1u), getParticleEnd(2u), getParticleEnd(3u));
            _out_data.length = interpolate1D(getParticleLength(0u), getParticleLength(1u), getParticleLength(2u), getParticleLength(3u));
        #endif

        #if defined (MeshEmitter)
            #if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
                _out_data.TBN = interpolateMat3(getVertexTBN(0u), getVertexTBN(1u), getVertexTBN(2u), getVertexTBN(3u));
            #else
                _out_data.normal = interpolate3D(getVertexNormal(0u), getVertexNormal(1u), getVertexNormal(2u), getVertexNormal(3u));
            #endif
        #endif

        #if defined (InitialColor_MODULE)
            _out_data.color = interpolate4D(getParticleColor(0u), getParticleColor(1u), getParticleColor(2u), getParticleColor(3u));
        #endif

        #if defined (SubUV_MODULE)
            _out_data.subUV = interpolate4D(getParticleSubUV(0u), getParticleSubUV(1u), getParticleSubUV(2u), getParticleSubUV(3u));
        #endif

        #if defined (CustomMaterial_MODULE)
            _out_data.properties = interpolate4D(getParticleProperties(0u), getParticleProperties(1u), getParticleProperties(2u), getParticleProperties(3u));
        #endif

        #if defined (InitialRotation_MODULE) || defined (Time_MODULE)
            _out_data.rotation = interpolate3D(getParticleRotation(0u), getParticleRotation(1u), getParticleRotation(2u), getParticleRotation(3u));
            _out_data.time = interpolate1D(getParticleTime(0u), getParticleTime(1u), getParticleTime(2u), getParticleTime(3u));
        #endif

        #if defined (SpriteEmitter) && defined (InitialVelocity_MODULE)
            _out_data.velocity = interpolate3D(getParticleVelocity(0u), getParticleVelocity(1u), getParticleVelocity(2u), getParticleVelocity(3u));
        #endif

        #if defined (BeamEmitter) && (defined (InitialVelocity_MODULE) || defined (InitialSize_Module))
            _out_data.velocity = interpolate3D(getParticleVelocity(0u), getParticleVelocity(1u), getParticleVelocity(2u), getParticleVelocity(3u));
            _out_data.size = interpolate1D(getParticleSize(0u), getParticleSize(1u), getParticleSize(2u), getParticleSize(3u));
        #endif

        #if defined (Lifetime_MODULE) || defined (Acceleration_MODULE)
            _out_data.acceleration = interpolate3D(getParticleAcceleration(0u), getParticleAcceleration(1u), getParticleAcceleration(2u), getParticleAcceleration(3u));
            _out_data.lifetime = interpolate1D(getParticleLifetime(0u), getParticleLifetime(1u), getParticleLifetime(2u), getParticleLifetime(3u));
        #endif

        #if defined (SpriteEmitter)
            _out_data.size = interpolate1D(getParticleSize(0u), getParticleSize(1u), getParticleSize(2u), getParticleSize(3u));
        #endif
    #else
        #if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
            _out_data.TBN = interpolateMat3(getVertexTBN(0u), getVertexTBN(1u), getVertexTBN(2u), getVertexTBN(3u));
        #else
            _out_data.normal = interpolate3D(getVertexNormal(0u), getVertexNormal(1u), getVertexNormal(2u), getVertexNormal(3u));
        #endif

        _out_data.world_position = interpolate3D(getVertexPosition(0u), getVertexPosition(1u), getVertexPosition(2u), getVertexPosition(3u));
        _out_data.uv = interpolate2D(getVertexUV(0u), getVertexUV(1u), getVertexUV(2u), getVertexUV(3u));
    #endif

    {
        vec3 world_position = _out_data.world_position;

        _MATERIAL_TESSELLATION_SNIPPET

        gl_Position = getViewProjection() * vec4(world_position, 1.0);
    }
}