/*
    these functions return default pipeline material final fragment properties

    vec4 getFragmentAlbedo();
    vec3 getFragmentNormal();
    vec3 getFragmentEmissive();

    float getFragmentAO();
    float getFragmentMetallic();
    float getFragmentRoughness();
*/

vec4 getFragmentAlbedo() {
    vec4 albedo = vec4(1.0);

    #if defined (MATERIAL_COLOR)
        albedo *= getMaterialColor();
    #endif

    #if defined (MATERIAL_DIFFUSE)
        albedo *= getMaterialDiffuse(getVertexUV());
    #endif

    #if defined (MATERIAL_BLENDMASK)
        if (getMaterialBlendMask(getVertexUV()) == 0.0) {
            discard;
        }
    #endif

    #if defined (EFFECT_MODEL) && defined (BeamEmitter) && defined (BeamSpeed_MODULE)
        if (distance(getVertexPosition(), getParticleStart()) / distance(getParticleStart(), getParticleEnd()) >= getParticleLength()) {
            discard;
        }
    #endif

    #if defined (EFFECT_MODEL) && defined (InitialColor_MODULE)
        albedo *= getParticleColor();
    #endif

    return albedo;
}

vec3 getFragmentNormal() {
    #if defined (SpriteEmitter) || defined (BeamEmitter)
        return getCameraPosition() - getVertexPosition();
    #else
        #if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
            vec3 normal = getMaterialNormal(getVertexUV());
            normal = normalize(normal * 2.0 - 1.0);
            normal = normalize(getVertexTBN() * normal);
        #else
            vec3 normal = normalize(getVertexNormal());
        #endif

        return normal;
    #endif
}

float getFragmentAO() {
    #if defined (MATERIAL_AMBIENT_OCCLUSION_TEXTURE)
        return getMaterialAmbientOcclusionTexture(getVertexUV());
    #else
        return 1.0;
    #endif
}

float getFragmentMetallic() {
    #if defined (MATERIAL_METALLIC_TEXTURE)
        return getMaterialMetallic(getVertexUV());
    #else
        return getMaterialMetallic();
    #endif
}

float getFragmentRoughness() {
    #if defined (MATERIAL_ROUGHNESS_TEXTURE)
        return getMaterialRoughness(getVertexUV());
    #else
        return getMaterialRoughness();
    #endif
}

vec3 getFragmentEmissive() {
    #if !defined (MATERIAL_EMISSIVE_COLOR) && !defined (MATERIAL_EMISSIVEMASK)
        return vec3(0.0);
    #else
        vec3 emissive = vec3(1.0);

        #if defined (MATERIAL_EMISSIVE_COLOR)
            emissive *= getMaterialEmissive();
        #endif

        #if defined (MATERIAL_EMISSIVEMASK)
            emissive *= getMaterialEmissiveMask(getVertexUV());
        #endif

        return emissive;
    #endif
}
