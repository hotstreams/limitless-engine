void InterfaceBlockPassThrough() {
        uint index = uint(gl_InvocationID);

#if defined (ENGINE_MATERIAL_EFFECT_MODEL)
        out_data[gl_InvocationID].world_position = getVertexPosition(index);

    #if !defined (SpriteEmitter)
        out_data[gl_InvocationID].uv = getVertexUV(index);
    #endif

    #if defined (BeamEmitter) && defined (BeamSpeed_MODULE)
        out_data[gl_InvocationID].start = getParticleStart(index);
        out_data[gl_InvocationID].end = getParticleEnd(index);
        out_data[gl_InvocationID].length = getParticleLength(index);
    #endif

    #if defined (MeshEmitter)
        #if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
            out_data[gl_InvocationID].TBN = getVertexTBN(index);
        #else
            out_data[gl_InvocationID].normal = getVertexNormal(index);
        #endif
    #endif

    #if defined (InitialColor_MODULE)
        out_data[gl_InvocationID].color = getParticleColor(index);
    #endif

    #if defined (SubUV_MODULE)
        out_data[gl_InvocationID].subUV = getParticleSubUV(index);
    #endif

    #if defined (CustomMaterial_MODULE)
        out_data[gl_InvocationID].properties = getParticleProperties(index);
    #endif

    #if defined (InitialRotation_MODULE) || defined (Time_MODULE)
        out_data[gl_InvocationID].rotation = getParticleRotation(index);
        out_data[gl_InvocationID].time = getParticleTime(index);
    #endif

    #if defined (SpriteEmitter) && defined (InitialVelocity_MODULE)
        out_data[gl_InvocationID].velocity = getParticleVelocity(index);
    #endif

    #if defined (BeamEmitter) && (defined (InitialVelocity_MODULE) || defined (InitialSize_Module))
        out_data[gl_InvocationID].velocity = getParticleVelocity(index);
        out_data[gl_InvocationID].size = getParticleSize(index);
    #endif

    #if defined (Lifetime_MODULE) || defined (Acceleration_MODULE)
        out_data[gl_InvocationID].acceleration = getParticleAcceleration(index);
        out_data[gl_InvocationID].lifetime = getParticleLifetime(index);
    #endif

    #if defined (SpriteEmitter)
        out_data[gl_InvocationID].size = getParticleSize(index);
    #endif
#else
    #if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
        out_data[gl_InvocationID].TBN = getVertexTBN(index);
    #else
        out_data[gl_InvocationID].normal = getVertexNormal(index);
    #endif

    out_data[gl_InvocationID].world_position = getVertexPosition(index);
    out_data[gl_InvocationID].uv = getVertexUV(index);
#endif
}
