/*
    order of buffer variables should match Limitless::ms::Property

    functions to get material properties

    uint getMaterialShadingModel();

    vec4 getMaterialColor();
    vec3 getMaterialEmissive();

    vec4 getMaterialDiffuse(vec2 uv);
    vec3 getMaterialNormal(vec2 uv);
    vec3 getMaterialDisplacement(vec2 uv);

    vec2 getMaterialTesselationFactor();

    float getMaterialEmissiveMask(vec2 uv);
    float getMaterialBlendMask(vec2 uv);
    float getMaterialMetallic(vec2 uv);
    float getMaterialRoughness(vec2 uv);

    float getMaterialShininess();
    float getMaterialMetallic();
    float getMaterialRoughness();
    float getMaterialRefraction();

*/

layout (std140) uniform material_buffer {
    #if defined (MATERIAL_COLOR)
        vec4 _material_color;
    #endif

    #if defined (MATERIAL_EMISSIVE_COLOR)
        vec4 _material_emissive;
    #endif

    #if defined (BINDLESS_TEXTURE)
        #if defined (MATERIAL_DIFFUSE)
            sampler2D _material_diffuse;
        #endif

        #if defined (MATERIAL_NORMAL)
            sampler2D _material_normal;
        #endif

        #if defined (MATERIAL_EMISSIVEMASK)
            sampler2D _material_emissive_mask;
        #endif

        #if defined (MATERIAL_BLENDMASK)
            sampler2D _material_blend_mask;
        #endif

        #if defined (MATERIAL_METALLIC_TEXTURE)
            sampler2D _material_metallic_texture;
        #endif

        #if defined (MATERIAL_ROUGHNESS_TEXTURE)
            sampler2D _material_roughness_texture;
        #endif

        #if defined (MATERIAL_AMBIENT_OCCLUSION_TEXTURE)
            sampler2D _material_ambient_occlusion_texture;
        #endif

        #if defined (MATERIAL_DISPLACEMENT)
            sampler2D _material_displacement;
        #endif
    #endif

    #if defined (MATERIAL_TESSELLATION_FACTOR)
        vec2 _material_tessellation_factor;
    #endif

    #if defined (MATERIAL_SHININESS)
        float _material_shininess;
    #endif

    #if defined (MATERIAL_METALLIC)
        float _material_metallic;
    #endif

    #if defined (MATERIAL_ROUGHNESS)
        float _material_roughness;
    #endif

    #if defined (MATERIAL_REFRACTION)
        float _material_refraction;
    #endif

    #if defined (BINDLESS_TEXTURE)
        _MATERIAL_SAMPLER_UNIFORMS
    #endif

    _MATERIAL_SCALAR_UNIFORMS

    uint _material_shading_model;
};

#if !defined (BINDLESS_TEXTURE)
    #if defined (MATERIAL_DIFFUSE)
        uniform sampler2D _material_diffuse;
    #endif

    #if defined (MATERIAL_NORMAL)
        uniform sampler2D _material_normal;
    #endif

    #if defined (MATERIAL_DISPLACEMENT)
        uniform sampler2D _material_displacement;
    #endif

    #if defined (MATERIAL_EMISSIVEMASK)
        uniform sampler2D _material_emissive_mask;
    #endif

    #if defined (MATERIAL_BLENDMASK)
        uniform sampler2D _material_blend_mask;
    #endif

    #if defined (MATERIAL_METALLIC_TEXTURE)
        uniform sampler2D _material_metallic_texture;
    #endif

    #if defined (MATERIAL_ROUGHNESS_TEXTURE)
        uniform sampler2D _material_roughness_texture;
    #endif

    #if defined (MATERIAL_AMBIENT_OCCLUSION_TEXTURE)
        uniform sampler2D _material_ambient_occlusion_texture;
    #endif

    _MATERIAL_SAMPLER_UNIFORMS
#endif

_MATERIAL_GLOBAL_DEFINITIONS

uint getMaterialShadingModel() {
    return _material_shading_model;
}

#if defined (MATERIAL_COLOR)
    vec4 getMaterialColor() {
        return _material_color;
    }
#endif

#if defined (MATERIAL_EMISSIVE_COLOR)
    vec3 getMaterialEmissive() {
        return _material_emissive.rgb;
    }
#endif

#if defined (MATERIAL_TESSELLATION_FACTOR)
    vec2 getMaterialTesselationFactor() {
        return _material_tessellation_factor;
    }
#endif

#if defined (MATERIAL_SHININESS)
    float getMaterialShininess() {
        return _material_shininess;
    }
#endif

float getMaterialMetallic() {
    #if defined (MATERIAL_METALLIC)
        return _material_metallic;
    #else
        return 0.1;
    #endif
}

float getMaterialRoughness() {
    #if defined (MATERIAL_ROUGHNESS)
        return _material_roughness;
    #else
        return 0.5;
    #endif
}

#if defined (MATERIAL_AMBIENT_OCCLUSION_TEXTURE)
    float getMaterialAmbientOcclusionTexture(vec2 uv) {
        return texture(_material_ambient_occlusion_texture, uv).r;
    }
#endif

#if defined (MATERIAL_REFRACTION)
    float getMaterialRefraction() {
        return _material_refraction;
    }
#endif

#if defined (MATERIAL_DIFFUSE)
    vec4 getMaterialDiffuse(vec2 uv) {
        return texture(_material_diffuse, uv);
    }
#endif

#if defined (MATERIAL_NORMAL)
    vec3 getMaterialNormal(vec2 uv) {
        return texture(_material_normal, uv).rgb;
    }
#endif

#if defined (MATERIAL_DISPLACEMENT)
    vec3 getMaterialDisplacement(vec2 uv) {
        return texture(_material_displacement, uv).rgb;
    }
#endif

#if defined (MATERIAL_EMISSIVEMASK)
    float getMaterialEmissiveMask(vec2 uv) {
        return texture(_material_emissive_mask, uv).r;
    }
#endif

#if defined (MATERIAL_BLENDMASK)
    float getMaterialBlendMask(vec2 uv) {
        return texture(_material_blend_mask, uv).r;
    }
#endif

#if defined (MATERIAL_METALLIC_TEXTURE)
    float getMaterialMetallic(vec2 uv) {
        return texture(_material_metallic_texture, uv).r;
    }
#endif

#if defined (MATERIAL_ROUGHNESS_TEXTURE)
    float getMaterialRoughness(vec2 uv) {
        return texture(_material_roughness_texture, uv).r;
    }
#endif
