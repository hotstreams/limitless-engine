#if defined (EFFECT_MODEL)
in _vertex_data {
#if defined (InitialColor_MODULE)
    vec4 color;
    #endif

    #if defined (SubUV_MODULE)
    vec4 subUV;
    #endif

    #if defined (CustomMaterial_MODULE)
    vec4 properties;
    #endif

    #if defined (Lifetime_MODULE) || defined (Acceleration_MODULE)
    vec3 acceleration;
    float lifetime;
    #endif

    #if defined (MeshEmitter)
    #if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
    mat3 TBN;
    #else
    vec3 normal;
    #endif
    #endif

    vec3 world_position;

    #if defined (SpriteEmitter)
    float size;
    #endif

    #if defined (InitialRotation_MODULE) || defined (Time_MODULE)
    vec3 rotation;
    float time;
    #endif

    #if (defined (SpriteEmitter) || defined (MeshEmitter)) && defined (InitialVelocity_MODULE)
    vec3 velocity;
    #endif

    #if defined (BeamEmitter) && (defined (InitialVelocity_MODULE) || defined (InitialSize_MODULE))
    vec3 velocity;
    float size;
    #endif

    #if defined (MeshEmitter)
    vec3 size;
    #endif

    #if defined (BeamEmitter) || defined (MeshEmitter)
    vec2 uv;
    #endif

    #if defined (BeamEmitter) && defined (BeamSpeed_MODULE)
    vec3 start;
    vec3 end;
    float length;
    #endif
} _in_data[];

#if defined (BeamEmitter) && defined (BeamSpeed_MODULE)
float getParticleLength(uint id) {
    return _in_data[id].length;
}

vec3 getParticleStart(uint id) {
    return _in_data[id].start;
}

vec3 getParticleEnd(uint id) {
    return _in_data[id].end;
}
    #endif

    #if defined (InitialColor_MODULE)
vec4 getParticleColor(uint id) {
    return _in_data[id].color;
}
    #endif

    #if defined (SubUV_MODULE)
vec4 getParticleSubUV(uint id) {
    return _in_data[id].subUV;
}
    #endif

    #if defined (CustomMaterial_MODULE)
vec4 getParticleProperties(uint id) {
    return _in_data[id].properties;
}
    #endif

    #if defined (Lifetime_MODULE) || defined (Acceleration_MODULE)
vec3 getParticleAcceleration(uint id) {
    return _in_data[id].acceleration;
}

float getParticleLifetime(uint id) {
    return _in_data[id].lifetime;
}
    #endif

vec3 getVertexPosition(uint id) {
    return _in_data[id].world_position;
}

    #if defined (SpriteEmitter)
float getParticleSize(uint id) {
    return _in_data[id].size;
}
    #endif

    #if defined (InitialRotation_MODULE) || defined (Time_MODULE)
vec3 getParticleRotation(uint id) {
    return _in_data[id].rotation;
}

float getParticleTime(uint id) {
    return _in_data[id].time;
}
    #endif

    #if defined (SpriteEmitter) && defined (InitialVelocity_MODULE)
vec3 getParticleVelocity(uint id) {
    return _in_data[id].velocity;
}
    #endif

    #if defined (BeamEmitter) && defined (InitialVelocity_MODULE)
vec3 getParticleVelocity(uint id) {
    return _in_data[id].velocity;
}

float getParticleSize(uint id) {
    return _in_data[id].size;
}
    #endif

    #if defined (BeamEmitter) || defined (MeshEmitter)
vec2 getVertexUV(uint id) {
    return _in_data[id].uv;
}
    #endif

    #if defined (SpriteEmitter)
vec2 getVertexUV() {
    vec2 uv = gl_PointCoord;

    return uv;
}
    #endif

    #if defined (MeshEmitter)
    #if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
mat3 getVertexTBN(uint id) {
    return _in_data[id].TBN;
}
    #else
vec3 getVertexNormal(uint id) {
    return _in_data[id].normal;
}
    #endif
    #endif

    #else
in _vertex_data {
#if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
    mat3 TBN;
    #else
    vec3 normal;
    #endif

    vec3 world_position;
    vec2 uv;
} _in_data[];

vec3 getVertexPosition(uint id) {
    return _in_data[id].world_position;
}

vec2 getVertexUV(uint id) {
    return _in_data[id].uv;
}

    #if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
mat3 getVertexTBN(uint id) {
    return _in_data[id].TBN;
}
    #else
vec3 getVertexNormal(uint id) {
    return _in_data[id].normal;
}
    #endif
    #endif