void InterfaceBlockPassThrough(vec3 world_position, vec2 uv, mat4 model_transform, vec3 normal) {
    #if defined (ENGINE_MATERIAL_EFFECT_MODEL)
        _out_data.world_position = world_position;

        #if !defined (SpriteEmitter)
            _out_data.uv = uv;
        #endif

        #if defined (BeamEmitter) && defined (BeamSpeed_MODULE)
            _out_data.start = getParticleStart();
            _out_data.end = getParticleEnd();
            _out_data.length = getParticleLength();
        #endif

        #if defined (SpriteEmitter)
            #if defined (InitialSize_MODULE)
                gl_PointSize = getProjection()[1][1] * getParticleSize() / gl_Position.w;
            #else
                gl_PointSize =  getProjection()[1][1] * 16.0 / gl_Position.w;
            #endif
        #endif

        #if defined (MeshEmitter)
            #if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
                mat3 normal_matrix = transpose(inverse(mat3(model_transform)));
                vec3 T = normalize(normal_matrix * getVertexTangent());
                vec3 N = normalize(normal_matrix * getVertexNormal());
                T = normalize(T - dot(T, N) * N);
                _out_data.tangent = T;
                _out_data.normal = N;
            #else
                _out_data.normal = transpose(inverse(mat3(model_transform))) * getVertexNormal();
            #endif
        #endif

        #if defined (InitialColor_MODULE)
            _out_data.color = getParticleColor();
        #endif

        #if defined (SubUV_MODULE)
            _out_data.subUV = getParticleSubUV();
        #endif

        #if defined (CustomMaterial_MODULE)
            _out_data.properties = getParticleProperties();
        #endif

        #if defined (InitialRotation_MODULE) || defined (Time_MODULE)
            _out_data.rotation = getParticleRotation();
            _out_data.time = getParticleTime();
        #endif

        #if defined (SpriteEmitter) && defined (InitialVelocity_MODULE)
            _out_data.velocity = getParticleVelocity();
        #endif

        #if defined (BeamEmitter) && (defined (InitialVelocity_MODULE) || defined (InitialSize_Module))
            _out_data.velocity = getParticleVelocity();
            _out_data.size = getParticleSize();
        #endif

        #if defined (Lifetime_MODULE) || defined (Acceleration_MODULE)
            _out_data.acceleration = getParticleAcceleration();
            _out_data.lifetime = getParticleLifetime();
        #endif

        #if defined (SpriteEmitter)
            _out_data.size = getParticleSize();
        #endif
    #else
        #if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
            mat3 normal_matrix = transpose(inverse(mat3(model_transform)));
            #if defined (ENGINE_MATERIAL_TERRAIN_MODEL)
                // nothing, gets calculated in terrain fragment shader
            #else
                vec3 T = normalize(normal_matrix * getVertexTangent());
                vec3 N = normalize(normal_matrix * getVertexNormal());
                T = normalize(T - dot(T, N) * N);
                _out_data.tangent = T;
                _out_data.normal = N;
            #endif
        #else
            #if defined (ENGINE_MATERIAL_TERRAIN_MODEL)
                _out_data.normal = transpose(inverse(mat3(model_transform))) * normal;
            #else
                _out_data.normal = transpose(inverse(mat3(model_transform))) * getVertexNormal();
            #endif
        #endif

        _out_data.world_position = world_position;
        _out_data.uv = uv;

        #if defined (ENGINE_MATERIAL_INSTANCED_MODEL)
           _out_data.instance_id = gl_InstanceID;
        #endif
    #endif
}
