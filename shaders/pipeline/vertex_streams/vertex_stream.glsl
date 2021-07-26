#if defined (EFFECT_MODEL)
    #if defined (SpriteEmitter)
        #include "./sprite_emitter.glsl"
    #endif

    #if defined (BeamEmitter)
        #include "./beam_emitter.glsl"
    #endif

    #if defined (MeshEmitter)
        #include "./mesh_emitter.glsl"
    #endif
#else
    #include "./model_mesh.glsl"
#endif
