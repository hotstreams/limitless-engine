#if defined (ENGINE_MATERIAL_REFRACTION)
    #include "../functions/refraction_sphere.glsl"

    uniform sampler2D _refraction_texture;

    vec3 getRefractionTextureValue(vec2 uv) {
        return texture(_refraction_texture, uv).rgb;
    }

    //TODO: refactor? pass Material/Shading COntext instead
    vec3 computeRefraction(const ShadingContext sctx, float IoR, float absorption) {
        #if !defined (ENGINE_MATERIAL_IOR)
           IoR = f0ToIor(sctx.F0.g);
        #endif

        const float airIor = 1.0;

        float etaIR = airIor / IoR;
        float etaRI = IoR / airIor;

        vec3 p = refractionSolidSphere(sctx.N, -normalize(getCameraPosition() - getVertexPosition()), etaIR, etaRI, getVertexPosition());
        vec4 p_cs = getViewProjection() * vec4(p, 1.0);
        p_cs.xy = p_cs.xy * (0.5 / p_cs.w) + 0.5;

        vec3 Ft = getRefractionTextureValue(p_cs.xy).rgb;

        #if !defined (ENGINE_MATERIAL_IOR)
            Ft *= sctx.diffuseColor;
        #endif

        #if defined (ENGINE_MATERIAL_ABSORPTION)
            Ft *= 1.0 - absorption;
        #endif

        return Ft;
    }
#endif
