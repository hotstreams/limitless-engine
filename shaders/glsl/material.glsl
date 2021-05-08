layout(std140) uniform material_buffer {
    #if defined(MATERIAL_COLOR)
        vec4 material_color;
    #endif

    #if defined(MATERIAL_EMISSIVE_COLOR)
        vec4 material_emissive_color;
    #endif

    #if defined(BINDLESS_TEXTURE)
        #if defined(MATERIAL_DIFFUSE)
            sampler2D material_diffuse;
        #endif

        #if defined(MATERIAL_NORMAL)
            sampler2D material_normal;
        #endif

        #if defined(MATERIAL_SPECULAR)
            sampler2D material_specular;
        #endif

        #if defined(MATERIAL_EMISSIVEMASK)
            sampler2D material_emissive_mask;
        #endif

        #if defined(MATERIAL_BLENDMASK)
            sampler2D material_blend_mask;
        #endif

        #if defined(MATERIAL_METALLIC_TEXTURE)
            sampler2D material_metallic_texture;
        #endif

        #if defined(MATERIAL_ROUGHNESS_TEXTURE)
            sampler2D material_roughness_texture;
        #endif

        #if defined(MATERIAL_DISPLACEMENT)
            sampler2D material_displacement;
        #endif
    #endif

    #if defined(MATERIAL_TESSELLATION_FACTOR)
        vec2 material_tessellation_factor;
    #endif

    #if defined(MATERIAL_SHININESS)
        float material_shininess;
    #endif

    #if defined(MATERIAL_METALLIC)
        float material_metallic;
    #endif

    #if defined(MATERIAL_ROUGHNESS)
        float material_roughness;
    #endif

    #if defined(BINDLESS_TEXTURE)
        Limitless::CustomMaterialSamplerUniforms
    #endif

    Limitless::CustomMaterialScalarUniforms

    // for tricky cases
    // when there are no samplers
    // and bindless texture is present
    bool empty;
};

#if !defined(BINDLESS_TEXTURE)
    #if defined(MATERIAL_DIFFUSE)
        uniform sampler2D material_diffuse;
    #endif

    #if defined(MATERIAL_SPECULAR)
        uniform sampler2D material_specular;
    #endif

    #if defined(MATERIAL_NORMAL)
        uniform sampler2D material_normal;
    #endif

    #if defined(MATERIAL_DISPLACEMENT)
        uniform sampler2D material_displacement;
    #endif

    #if defined(MATERIAL_EMISSIVEMASK)
        uniform sampler2D material_emissive_mask;
    #endif

    #if defined(MATERIAL_BLENDMASK)
        uniform sampler2D material_blend_mask;
    #endif

    #if defined(MATERIAL_METALLIC_TEXTURE)
        uniform sampler2D material_metallic_texture;
    #endif

    #if defined(MATERIAL_ROUGHNESS_TEXTURE)
        uniform sampler2D material_roughness_texture;
    #endif

    Limitless::CustomMaterialSamplerUniforms
#endif

Limitless::CustomMaterialGlobalDefinitions
