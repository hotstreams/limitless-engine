void InterfaceBlockPassThrough() {
#if defined (EFFECT_MODEL)
        _out_data[gl_InvocationID].world_position = getVertexPosition(gl_InvocationID);

    #if !defined (SpriteEmitter)
        _out_data[gl_InvocationID].uv = getVertexUV(gl_InvocationID);
    #endif

    #if defined (BeamEmitter) && defined (BeamSpeed_MODULE)
        _out_data[gl_InvocationID].start = getParticleStart(gl_InvocationID);
        _out_data[gl_InvocationID].end = getParticleEnd(gl_InvocationID);
        _out_data[gl_InvocationID].length = getParticleLength(gl_InvocationID);
    #endif

    #if defined (MeshEmitter)
        #if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
            _out_data[gl_InvocationID].TBN = getVertexTBN(gl_InvocationID);
        #else
            _out_data[gl_InvocationID].normal = getVertexNormal(gl_InvocationID);
        #endif
    #endif

    #if defined (InitialColor_MODULE)
        _out_data[gl_InvocationID].color = getParticleColor(gl_InvocationID);
    #endif

    #if defined (SubUV_MODULE)
        _out_data[gl_InvocationID].subUV = getParticleSubUV(gl_InvocationID);
    #endif

    #if defined (CustomMaterial_MODULE)
        _out_data[gl_InvocationID].properties = getParticleProperties(gl_InvocationID);
    #endif

    #if defined (InitialRotation_MODULE) || defined (Time_MODULE)
        _out_data[gl_InvocationID].rotation = getParticleRotation(gl_InvocationID);
        _out_data[gl_InvocationID].time = getParticleTime(gl_InvocationID);
    #endif

    #if defined (SpriteEmitter) && defined (InitialVelocity_MODULE)
        _out_data[gl_InvocationID].velocity = getParticleVelocity(gl_InvocationID);
    #endif

    #if defined (BeamEmitter) && (defined (InitialVelocity_MODULE) || defined (InitialSize_Module))
        _out_data[gl_InvocationID].velocity = getParticleVelocity(gl_InvocationID);
        _out_data[gl_InvocationID].size = getParticleSize(gl_InvocationID);
    #endif

    #if defined (Lifetime_MODULE) || defined (Acceleration_MODULE)
        _out_data[gl_InvocationID].acceleration = getParticleAcceleration(gl_InvocationID);
        _out_data[gl_InvocationID].lifetime = getParticleLifetime(gl_InvocationID);
    #endif

    #if defined (SpriteEmitter)
        _out_data[gl_InvocationID].size = getParticleSize(gl_InvocationID);
    #endif
#else
    #if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
        _out_data[gl_InvocationID].TBN = getVertexTBN(gl_InvocationID);
    #else
        _out_data[gl_InvocationID].normal = getVertexNormal(gl_InvocationID);
    #endif

    _out_data[gl_InvocationID].world_position = getVertexPosition(gl_InvocationID);
    _out_data[gl_InvocationID].uv = getVertexUV(gl_InvocationID);
#endif
}
