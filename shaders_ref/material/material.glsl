layout (std140) uniform MATERIAL_BUFFER {
#if defined (ENGINE_MATERIAL_COLOR)
    vec4 _material_color;
#endif

#if defined (ENGINE_MATERIAL_EMISSIVE_COLOR)
    vec4 _material_emissive_color;
#endif

#if defined (ENGINE_EXT_BINDLESS_TEXTURE)
#if defined (ENGINE_MATERIAL_DIFFUSE_TEXTURE)
    sampler2D _material_diffuse_texture;
#endif

#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE)
    sampler2D _material_normal_texture;
#endif

#if defined (ENGINE_MATERIAL_EMISSIVEMASK_TEXTURE)
    sampler2D _material_emissive_mask_texture;
#endif

#if defined (ENGINE_MATERIAL_BLENDMASK_TEXTURE)
    sampler2D _material_blend_mask_texture;
#endif

#if defined (ENGINE_MATERIAL_METALLIC_TEXTURE)
    sampler2D _material_metallic_texture;
#endif

#if defined (ENGINE_MATERIAL_ROUGHNESS_TEXTURE)
    sampler2D _material_roughness_texture;
#endif

#if defined (ENGINE_MATERIAL_AMBIENT_OCCLUSION_TEXTURE)
    sampler2D _material_ambient_occlusion_texture;
#endif

#if defined (ENGINE_MATERIAL_ORM_TEXTURE)
    sampler2D _material_orm_texture;
#endif
#endif

#if defined (ENGINE_MATERIAL_TESSELLATION_FACTOR)
    vec2 _material_tessellation_factor;
#endif

#if defined (ENGINE_MATERIAL_METALLIC)
    float _material_metallic;
#endif

#if defined (ENGINE_MATERIAL_ROUGHNESS)
    float _material_roughness;
#endif

#if defined (ENGINE_MATERIAL_REFRACTION)
#if defined (ENGINE_MATERIAL_IOR)
    float _material_ior;
#endif

#if defined (ENGINE_MATERIAL_ABSORPTION)
    float _material_absorption;
#endif
#endif

#if defined (ENGINE_EXT_BINDLESS_TEXTURE)
    ENGINE_MATERIAL_CUSTOM_SAMPLERS
#endif

    ENGINE_MATERIAL_CUSTOM_SCALARS

    uint _material_shading_model;
};

#if !defined (ENGINE_EXT_BINDLESS_TEXTURE)
#if defined (ENGINE_MATERIAL_DIFFUSE_TEXTURE)
    uniform sampler2D _material_diffuse_texture;
#endif

#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE)
    uniform sampler2D _material_normal_texture;
#endif

#if defined (ENGINE_MATERIAL_EMISSIVEMASK_TEXTURE)
    uniform sampler2D _material_emissive_mask_texture;
#endif

#if defined (ENGINE_MATERIAL_BLENDMASK_TEXTURE)
    uniform sampler2D _material_blend_mask_texture;
#endif

#if defined (ENGINE_MATERIAL_METALLIC_TEXTURE)
    uniform sampler2D _material_metallic_texture;
#endif

#if defined (ENGINE_MATERIAL_ROUGHNESS_TEXTURE)
    uniform sampler2D _material_roughness_texture;
#endif

#if defined (ENGINE_MATERIAL_AMBIENT_OCCLUSION_TEXTURE)
    uniform sampler2D _material_ambient_occlusion_texture;
#endif

#if defined (ENGINE_MATERIAL_ORM_TEXTURE)
    uniform sampler2D _material_orm_texture;
#endif

    ENGINE_MATERIAL_CUSTOM_SAMPLERS
#endif

ENGINE_MATERIAL_GLOBAL_DEFINITIONS

/**
  *     Public material access API
  *
  *     Do NOT forget to edit API.md on public API change
  */
#if defined (ENGINE_MATERIAL_COLOR)
    vec4 getMaterialColor() {
        return _material_color;
    }
#endif

#if defined (ENGINE_MATERIAL_EMISSIVE_COLOR)
    vec3 getMaterialEmissiveColor() {
        return _material_emissive_color.rgb;
    }
#endif

#if defined (ENGINE_MATERIAL_TESSELLATION_FACTOR)
    vec2 getMaterialTesselationFactor() {
        return _material_tessellation_factor;
    }
#endif

#if defined (ENGINE_MATERIAL_METALLIC)
    float getMaterialMetallic() {
        return _material_metallic;
    }
#endif

#if defined (ENGINE_MATERIAL_ROUGHNESS)
    float getMaterialRoughness() {
        return _material_roughness;
    }
#endif

#if defined (ENGINE_MATERIAL_AMBIENT_OCCLUSION_TEXTURE)
    float getMaterialAmbientOcclusion(vec2 uv) {
        return texture(_material_ambient_occlusion_texture, uv).r;
    }
#endif

#if defined (ENGINE_MATERIAL_ORM_TEXTURE)
    vec3 getMaterialORM(vec2 uv) {
        return texture(_material_orm_texture, uv).rgb;
    }
#endif

#if defined (ENGINE_MATERIAL_REFRACTION)
#if defined (ENGINE_MATERIAL_IOR)
    float getMaterialIOR() {
        return _material_ior;
    }
#endif

#if defined (ENGINE_MATERIAL_ABSORPTION)
    float getMaterialAbsorption() {
        return _material_absorption;
    }
#endif
#endif

#if defined (ENGINE_MATERIAL_DIFFUSE_TEXTURE)
    vec4 getMaterialDiffuse(vec2 uv) {
        return texture(_material_diffuse_texture, uv);
    }
#endif

#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE)
    vec3 getMaterialNormal(vec2 uv) {
        return texture(_material_normal_texture, uv).xyz;
    }
#endif

#if defined (ENGINE_MATERIAL_EMISSIVEMASK_TEXTURE)
    vec3 getMaterialEmissiveMask(vec2 uv) {
        return texture(_material_emissive_mask_texture, uv).rgb;
    }
#endif

#if defined (ENGINE_MATERIAL_BLENDMASK_TEXTURE)
    float getMaterialBlendMask(vec2 uv) {
        return texture(_material_blend_mask_texture, uv).r;
    }
#endif

#if defined (ENGINE_MATERIAL_METALLIC_TEXTURE)
    float getMaterialMetallic(vec2 uv) {
        return texture(_material_metallic_texture, uv).r;
    }
#endif

#if defined (ENGINE_MATERIAL_ROUGHNESS_TEXTURE)
    float getMaterialRoughness(vec2 uv) {
        return texture(_material_roughness_texture, uv).r;
    }
#endif

uint getMaterialShadingModel() {
    return _material_shading_model;
}
