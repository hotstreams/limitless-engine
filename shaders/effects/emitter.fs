Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::EmitterType

#include "../glsl/material.glsl"
#include "../glsl/scene.glsl"

in vertex_data {
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
    #endif

    #if defined(BeamEmitter) || defined(MeshEmitter)
        vec2 uv;
    #endif
} in_data;

#if defined(InitialColor_MODULE)
    vec4 getParticleColor() {
        return in_data.color;
    }
#endif

#if defined(SubUV_MODULE)
    vec4 getParticleSubUV() {
        return in_data.subUV;
    }
#endif

#if defined(CustomMaterial_MODULE)
    vec4 getParticleProperties() {
        return in_data.properties;
    }
#endif

#if defined(Lifetime_MODULE) || defined(Acceleration_MODULE)
    vec3 getParticleAcceleration() {
        return in_data.acceleration;
    }

    float getParticleLifetime() {
        return in_data.lifetime;
    }
#endif

vec3 getParticlePosition() {
    return in_data.position;
}

#if defined(SpriteEmitter)
    float getParticleSize() {
        return in_data.size;
    }
#endif

#if defined(InitialRotation_MODULE) || defined(Time_MODULE)
    vec3 getParticleRotation() {
        return in_data.rotation;
    }

    float getParticleTime() {
        return in_data.time;
    }
#endif

#if defined(SpriteEmitter)
    #if defined(InitialVelocity_MODULE)
        vec3 getParticleVelocity() {
            return in_data.velocity;
        }
    #endif
#endif

#if defined(BeamEmitter)
    #if defined(InitialVelocity_MODULE)
        vec3 getParticleVelocity() {
            return in_data.velocity;
        }

        float getParticleSize() {
            return in_data.size;
        }
    #endif
#endif

#if defined(BeamEmitter) || defined(MeshEmitter)
    vec2 getUV() {
        return in_data.uv;
    }
#endif

#if defined(SpriteEmitter)
    vec2 getUV() {
        vec2 uv = gl_PointCoord;

        #if defined(InitialRotation_MODULE)
            vec2 center = vec2(0.5, 0.5);
            vec2 translated = uv - center;
            mat2 rotation = mat2(cos(getParticleRotation().x), sin(getParticleRotation().x), -sin(getParticleRotation().x), cos(getParticleRotation().x));
            translated = rotation * translated;
            translated = translated + center;
            uv = clamp(translated, 0.0, 1.0);
        #endif

        return uv;
    }
#endif

out vec4 color;

void main() {
    vec2 uv = getUV();

    #if defined(SubUV_MODULE)
        uv = uv * in_data.subUV.xy + in_data.subUV.zw;
    #endif

    #include "../glsl/material_variables.glsl"

    Limitless::CustomMaterialFragmentCode

    // computing final color
    vec4 fragment_color = vec4(1.0);
    #if defined(InitialColor_MODULE)
        fragment_color *= getParticleColor();
    #endif

    #include "../glsl/material_computation.glsl"

    color = fragment_color;
}
