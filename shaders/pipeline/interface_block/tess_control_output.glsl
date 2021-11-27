#if defined (EFFECT_MODEL)
    out _vertex_data {
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
    } _out_data[];

#else
    out _vertex_data {
        #if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
            mat3 TBN;
        #else
            vec3 normal;
        #endif

        vec3 world_position;
        vec2 uv;
    } _out_data[];
#endif