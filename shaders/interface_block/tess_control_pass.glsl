void InterfaceBlockPassThrough() {
        uint index = uint(gl_InvocationID);

#if defined (ENGINE_MATERIAL_EFFECT_MODEL)
        _out_data[gl_InvocationID].world_position = getVertexPosition(index);

    #if !defined (SpriteEmitter)
        _out_data[gl_InvocationID].uv = getVertexUV(index);
    #endif

    #if defined (BeamEmitter) && defined (BeamSpeed_MODULE)
        _out_data[gl_InvocationID].start = getParticleStart(index);
        _out_data[gl_InvocationID].end = getParticleEnd(index);
        _out_data[gl_InvocationID].length = getParticleLength(index);
    #endif

    #if defined (MeshEmitter)
        #if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
            _out_data[gl_InvocationID].TBN = getVertexTBN(index);
        #else
            _out_data[gl_InvocationID].normal = getVertexNormal(index);
        #endif
    #endif

    #if defined (InitialColor_MODULE)
        _out_data[gl_InvocationID].color = getParticleColor(index);
    #endif

    #if defined (SubUV_MODULE)
        _out_data[gl_InvocationID].subUV = getParticleSubUV(index);
    #endif

    #if defined (CustomMaterial_MODULE)
        _out_data[gl_InvocationID].properties = getParticleProperties(index);
    #endif

    #if defined (InitialRotation_MODULE) || defined (Time_MODULE)
        _out_data[gl_InvocationID].rotation = getParticleRotation(index);
        _out_data[gl_InvocationID].time = getParticleTime(index);
    #endif

    #if defined (SpriteEmitter) && defined (InitialVelocity_MODULE)
        _out_data[gl_InvocationID].velocity = getParticleVelocity(index);
    #endif

    #if defined (BeamEmitter) && (defined (InitialVelocity_MODULE) || defined (InitialSize_Module))
        _out_data[gl_InvocationID].velocity = getParticleVelocity(index);
        _out_data[gl_InvocationID].size = getParticleSize(index);
    #endif

    #if defined (Lifetime_MODULE) || defined (Acceleration_MODULE)
        _out_data[gl_InvocationID].acceleration = getParticleAcceleration(index);
        _out_data[gl_InvocationID].lifetime = getParticleLifetime(index);
    #endif

    #if defined (SpriteEmitter)
        _out_data[gl_InvocationID].size = getParticleSize(index);
    #endif
#else
    #if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
        _out_data[gl_InvocationID].TBN = getVertexTBN(index);
    #else
        _out_data[gl_InvocationID].normal = getVertexNormal(index);
    #endif

    _out_data[gl_InvocationID].world_position = getVertexPosition(index);
    _out_data[gl_InvocationID].uv = getVertexUV(index);
#endif
}
