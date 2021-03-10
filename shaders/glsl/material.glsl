layout(std140) uniform material_buffer {
    #ifdef MATERIAL_COLOR
        vec4 material_color;
    #endif

    #ifdef MATERIAL_EMISSIVE_COLOR
        vec4 material_emissive_color;
    #endif

    #ifdef BINDLESS_TEXTURE
        #ifdef MATERIAL_DIFFUSE
            sampler2D material_diffuse;
        #endif

        #ifdef MATERIAL_SPECULAR
            sampler2D material_specular;
        #endif

        #ifdef MATERIAL_NORMAL
            sampler2D material_normal;
        #endif

        #ifdef MATERIAL_DISPLACEMENT
            sampler2D material_displacement;
        #endif

        #ifdef MATERIAL_EMISSIVEMASK
            sampler2D material_emissive_mask;
        #endif

        #ifdef MATERIAL_BLENDMASK
            sampler2D material_blend_mask;
        #endif

        #ifdef MATERIAL_METALLIC_TEXTURE
            sampler2D material_metallic_texture;
        #endif

        #ifdef MATERIAL_ROUGHNESS_TEXTURE
            sampler2D material_roughness_texture;
        #endif

        #ifdef CUSTOM_MATERIAL
            LimitlessEngine::CustomMaterialSamplerUniforms
        #endif
    #endif

    #ifdef MATERIAL_SHININESS
        float material_shininess;
    #endif

    #ifdef MATERIAL_METALLIC
        float material_metallic;
    #endif

    #ifdef MATERIAL_ROUGHNESS
        float material_roughness;
    #endif

    #ifdef CUSTOM_MATERIAL
        LimitlessEngine::CustomMaterialScalarUniforms
    #endif

    // for cases if there is no scalar values in buffer and no bindless textures
    // GLSL cannot compile empty uniform block :/
    bool empty;
};

#ifndef BINDLESS_TEXTURE
    #ifdef MATERIAL_DIFFUSE
        uniform sampler2D material_diffuse;
    #endif

    #ifdef MATERIAL_SPECULAR
        uniform sampler2D material_specular;
    #endif

    #ifdef MATERIAL_NORMAL
        uniform sampler2D material_normal;
    #endif

    #ifdef MATERIAL_DISPLACEMENT
        uniform sampler2D material_displacement;
    #endif

    #ifdef MATERIAL_EMISSIVEMASK
        uniform sampler2D material_emissive_mask;
    #endif

    #ifdef MATERIAL_BLENDMASK
        uniform sampler2D material_blend_mask;
    #endif

    #ifdef MATERIAL_METALLIC_TEXTURE
        uniform sampler2D material_metallic_texture;
    #endif

    #ifdef MATERIAL_ROUGHNESS_TEXTURE
        uniform sampler2D material_roughness_texture;
    #endif

    #ifdef CUSTOM_MATERIAL
        LimitlessEngine::CustomMaterialSamplerUniforms
    #endif
#endif
