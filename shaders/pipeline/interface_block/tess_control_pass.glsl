void InterfaceBlockPassThrough() {
        uint index = uint(gl_InvocationID);

#if defined (EFFECT_MODEL)
        _out_data[index].world_position = getVertexPosition(index);

    #if !defined (SpriteEmitter)
        _out_data[index].uv = getVertexUV(index);
    #endif

    #if defined (BeamEmitter) && defined (BeamSpeed_MODULE)
        _out_data[index].start = getParticleStart(index);
        _out_data[index].end = getParticleEnd(index);
        _out_data[index].length = getParticleLength(index);
    #endif

    #if defined (MeshEmitter)
        #if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
            _out_data[index].TBN = getVertexTBN(index);
        #else
            _out_data[index].normal = getVertexNormal(index);
        #endif
    #endif

    #if defined (InitialColor_MODULE)
        _out_data[index].color = getParticleColor(index);
    #endif

    #if defined (SubUV_MODULE)
        _out_data[index].subUV = getParticleSubUV(index);
    #endif

    #if defined (CustomMaterial_MODULE)
        _out_data[index].properties = getParticleProperties(index);
    #endif

    #if defined (InitialRotation_MODULE) || defined (Time_MODULE)
        _out_data[index].rotation = getParticleRotation(index);
        _out_data[index].time = getParticleTime(index);
    #endif

    #if defined (SpriteEmitter) && defined (InitialVelocity_MODULE)
        _out_data[index].velocity = getParticleVelocity(index);
    #endif

    #if defined (BeamEmitter) && (defined (InitialVelocity_MODULE) || defined (InitialSize_Module))
        _out_data[index].velocity = getParticleVelocity(index);
        _out_data[index].size = getParticleSize(index);
    #endif

    #if defined (Lifetime_MODULE) || defined (Acceleration_MODULE)
        _out_data[index].acceleration = getParticleAcceleration(index);
        _out_data[index].lifetime = getParticleLifetime(index);
    #endif

    #if defined (SpriteEmitter)
        _out_data[index].size = getParticleSize(index);
    #endif
#else
    #if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
        _out_data[index].TBN = getVertexTBN(index);
    #else
        _out_data[index].normal = getVertexNormal(index);
    #endif

    _out_data[index].world_position = getVertexPosition(index);
    _out_data[index].uv = getVertexUV(index);
#endif
}
