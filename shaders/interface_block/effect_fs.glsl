in vertex_data {
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
        #if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
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
} in_data;

#if defined (BeamEmitter) && defined (BeamSpeed_MODULE)
    float getParticleLength() {
        return in_data.length;
    }

    vec3 getParticleStart() {
        return in_data.start;
    }

    vec3 getParticleEnd() {
        return in_data.end;
    }
#endif

#if defined (InitialColor_MODULE)
    vec4 getParticleColor() {
        return in_data.color;
    }
#endif

#if defined (SubUV_MODULE)
    vec4 getParticleSubUV() {
        return in_data.subUV;
    }
#endif

#if defined (CustomMaterial_MODULE)
    vec4 getParticleProperties() {
        return in_data.properties;
    }
#endif

#if defined (Lifetime_MODULE) || defined (Acceleration_MODULE)
    vec3 getParticleAcceleration() {
        return in_data.acceleration;
    }

    float getParticleLifetime() {
        return in_data.lifetime;
    }
#endif

vec3 getVertexPosition() {
    return in_data.world_position;
}

#if defined (SpriteEmitter)
    float getParticleSize() {
        return in_data.size;
    }
#endif

#if defined (InitialRotation_MODULE) || defined (Time_MODULE)
    vec3 getParticleRotation() {
        return in_data.rotation;
    }

    float getParticleTime() {
        return in_data.time;
    }
#endif

#if defined (SpriteEmitter) && defined (InitialVelocity_MODULE)
    vec3 getParticleVelocity() {
        return in_data.velocity;
    }
#endif

#if defined (BeamEmitter) && defined (InitialVelocity_MODULE)
    vec3 getParticleVelocity() {
        return in_data.velocity;
    }

    float getParticleSize() {
        return in_data.size;
    }
#endif

#if defined (BeamEmitter) || defined (MeshEmitter)
    vec2 getVertexUV() {
        return in_data.uv;
    }
#endif

#if defined (SpriteEmitter)
    vec2 getVertexUV() {
        vec2 uv = gl_PointCoord;

        #if defined (InitialRotation_MODULE)
            vec2 center = vec2(0.5, 0.5);
            vec2 translated = uv - center;
            mat2 rotation = mat2(cos(getParticleRotation().x), sin(getParticleRotation().x), -sin(getParticleRotation().x), cos(getParticleRotation().x));
            translated = rotation * translated;
            translated = translated + center;
            uv = clamp(translated, 0.0, 1.0);
        #endif

        #if defined (SubUV_MODULE)
            uv = uv * getParticleSubUV().xy + getParticleSubUV().zw;
        #endif

        return uv;
    }
#endif

#if defined (MeshEmitter)
    #if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
        mat3 getVertexTBN() {
            return in_data.TBN;
        }
    #else
       vec3 getVertexNormal() {
            return in_data.normal;
        }
    #endif
#endif
