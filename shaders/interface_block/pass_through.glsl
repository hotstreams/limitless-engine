void InterfaceBlockPassThrough(vec3 world_position, vec2 uv, mat4 model_transform) {
    #if defined (ENGINE_MATERIAL_EFFECT_MODEL)
        out_data.world_position = world_position;

        #if !defined (SpriteEmitter)
            out_data.uv = uv;
        #endif

        #if defined (BeamEmitter) && defined (BeamSpeed_MODULE)
            out_data.start = getParticleStart();
            out_data.end = getParticleEnd();
            out_data.length = getParticleLength();
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
                out_data.TBN = getModelTBN(model_transform);
            #else
                out_data.normal = transpose(inverse(mat3(model_transform))) * getVertexNormal();
            #endif
        #endif

        #if defined (InitialColor_MODULE)
            out_data.color = getParticleColor();
        #endif

        #if defined (SubUV_MODULE)
            out_data.subUV = getParticleSubUV();
        #endif

        #if defined (CustomMaterial_MODULE)
            out_data.properties = getParticleProperties();
        #endif

        #if defined (InitialRotation_MODULE) || defined (Time_MODULE)
            out_data.rotation = getParticleRotation();
            out_data.time = getParticleTime();
        #endif

        #if defined (SpriteEmitter) && defined (InitialVelocity_MODULE)
            out_data.velocity = getParticleVelocity();
        #endif

        #if defined (BeamEmitter) && (defined (InitialVelocity_MODULE) || defined (InitialSize_Module))
            out_data.velocity = getParticleVelocity();
            out_data.size = getParticleSize();
        #endif

        #if defined (Lifetime_MODULE) || defined (Acceleration_MODULE)
            out_data.acceleration = getParticleAcceleration();
            out_data.lifetime = getParticleLifetime();
        #endif

        #if defined (SpriteEmitter)
            out_data.size = getParticleSize();
        #endif
    #else
        #if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
            out_data.TBN = getModelTBN(model_transform);
        #else
            out_data.normal = transpose(inverse(mat3(model_transform))) * getVertexNormal();
        #endif

        out_data.world_position = world_position;
        out_data.uv = uv;

        #if defined (ENGINE_MATERIAL_TERRAIN_MODEL)
            out_data.uv1 = getVertexUV1();
            out_data.uv2 = getVertexUV2();
            out_data.uv3 = getVertexUV3();
            out_data.mask = getVertexColor();
            out_data.current = getVertexTileCurrent();
            out_data.types = getVertexTileType();
        #endif

        #if defined (ENGINE_MATERIAL_INSTANCED_MODEL)
           out_data.instance_id = gl_InstanceID;
        #endif
    #endif
}
