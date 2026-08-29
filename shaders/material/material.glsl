layout (std140) uniform MATERIAL_BUFFER {
#if defined (ENGINE_MATERIAL_COLOR)
    vec4 material_color;
#endif

#if defined (ENGINE_MATERIAL_EMISSIVE_COLOR)
    vec3 material_emissive_color;
#endif

#if defined (ENGINE_EXT_BINDLESS_TEXTURE)
#if defined (ENGINE_MATERIAL_DIFFUSE_TEXTURE)
    sampler2D material_diffuse_texture;
#endif

#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE)
    sampler2D material_normal_texture;
#endif

#if defined (ENGINE_MATERIAL_EMISSIVEMASK_TEXTURE)
    sampler2D material_emissive_mask_texture;
#endif

#if defined (ENGINE_MATERIAL_BLENDMASK_TEXTURE)
    sampler2D material_blend_mask_texture;
#endif

#if defined (ENGINE_MATERIAL_METALLIC_TEXTURE)
    sampler2D material_metallic_texture;
#endif

#if defined (ENGINE_MATERIAL_ROUGHNESS_TEXTURE)
    sampler2D material_roughness_texture;
#endif

#if defined (ENGINE_MATERIAL_AMBIENT_OCCLUSION_TEXTURE)
    sampler2D material_ambient_occlusion_texture;
#endif

#if defined (ENGINE_MATERIAL_ORM_TEXTURE)
    sampler2D material_orm_texture;
#endif
#endif

#if defined (ENGINE_MATERIAL_METALLIC)
    float material_metallic;
#endif

#if defined (ENGINE_MATERIAL_ROUGHNESS)
    float material_roughness;
#endif

#if defined (ENGINE_MATERIAL_REFRACTION)
#if defined (ENGINE_MATERIAL_IOR)
    float material_ior;
#endif

#if defined (ENGINE_MATERIAL_ABSORPTION)
    float material_absorption;
#endif

#if defined (ENGINE_MATERIAL_MICROTHICKNESS)
    float material_microthickness;
#endif

#if defined (ENGINE_MATERIAL_THICKNESS)
    float material_thickness;
#endif
#endif

#if defined (ENGINE_MATERIAL_REFLECTANCE)
    float material_reflectance;
#endif

#if defined (ENGINE_MATERIAL_TRANSMISSION)
    float material_transmission;
#endif

#if defined (ENGINE_EXT_BINDLESS_TEXTURE)
    ENGINE_MATERIAL_CUSTOM_SAMPLERS
#endif

    ENGINE_MATERIAL_CUSTOM_SCALARS

    uint material_shading_model;
};

#if !defined (ENGINE_EXT_BINDLESS_TEXTURE)
#if defined (ENGINE_MATERIAL_DIFFUSE_TEXTURE)
    uniform sampler2D material_diffuse_texture;
#endif

#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE)
    uniform sampler2D material_normal_texture;
#endif

#if defined (ENGINE_MATERIAL_EMISSIVEMASK_TEXTURE)
    uniform sampler2D material_emissive_mask_texture;
#endif

#if defined (ENGINE_MATERIAL_BLENDMASK_TEXTURE)
    uniform sampler2D material_blend_mask_texture;
#endif

#if defined (ENGINE_MATERIAL_METALLIC_TEXTURE)
    uniform sampler2D material_metallic_texture;
#endif

#if defined (ENGINE_MATERIAL_ROUGHNESS_TEXTURE)
    uniform sampler2D material_roughness_texture;
#endif

#if defined (ENGINE_MATERIAL_AMBIENT_OCCLUSION_TEXTURE)
    uniform sampler2D material_ambient_occlusion_texture;
#endif

#if defined (ENGINE_MATERIAL_ORM_TEXTURE)
    uniform sampler2D material_orm_texture;
#endif

    ENGINE_MATERIAL_CUSTOM_SAMPLERS
#endif

/**
  *     Public material access API
  *
  *     Do NOT forget to edit API.md on public API change
  */
#if defined (ENGINE_MATERIAL_COLOR)
    vec4 getMaterialColor() {
        return material_color;
    }
#endif

#if defined (ENGINE_MATERIAL_EMISSIVE_COLOR)
    vec3 getMaterialEmissiveColor() {
        return material_emissive_color.rgb;
    }
#endif

#if defined (ENGINE_MATERIAL_METALLIC)
    float getMaterialMetallic() {
        return material_metallic;
    }
#endif

#if defined (ENGINE_MATERIAL_ROUGHNESS)
    float getMaterialRoughness() {
        return material_roughness;
    }
#endif

#if defined (ENGINE_MATERIAL_AMBIENT_OCCLUSION_TEXTURE)
    float getMaterialAmbientOcclusion(vec2 uv) {
        return texture(material_ambient_occlusion_texture, uv).r;
    }
#endif

#if defined (ENGINE_MATERIAL_ORM_TEXTURE)
    vec3 getMaterialORM(vec2 uv) {
        return texture(material_orm_texture, uv).rgb;
    }
#endif

#if defined (ENGINE_MATERIAL_REFRACTION)
#if defined (ENGINE_MATERIAL_IOR)
    float getMaterialIOR() {
        return material_ior;
    }
#endif

#if defined (ENGINE_MATERIAL_ABSORPTION)
    float getMaterialAbsorption() {
        return material_absorption;
    }
#endif

#if defined (ENGINE_MATERIAL_MICROTHICKNESS)
    float getMaterialMicrothickness() {
        return material_microthickness;
    }
#endif

#if defined (ENGINE_MATERIAL_THICKNESS)
    float getMaterialThickness() {
        return material_thickness;
    }
#endif
#endif

#if defined (ENGINE_MATERIAL_DIFFUSE_TEXTURE)
    vec4 getMaterialDiffuse(vec2 uv) {
        return texture(material_diffuse_texture, uv);
    }
#endif

#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE)
    vec3 getMaterialNormal(vec2 uv) {
        return texture(material_normal_texture, uv).xyz;
    }
#endif

#if defined (ENGINE_MATERIAL_EMISSIVEMASK_TEXTURE)
    vec3 getMaterialEmissiveMask(vec2 uv) {
        return texture(material_emissive_mask_texture, uv).rgb;
    }
#endif

#if defined (ENGINE_MATERIAL_BLENDMASK_TEXTURE)
    float getMaterialBlendMask(vec2 uv) {
        return texture(material_blend_mask_texture, uv).r;
    }
#endif

#if defined (ENGINE_MATERIAL_METALLIC_TEXTURE)
    float getMaterialMetallic(vec2 uv) {
        return texture(material_metallic_texture, uv).r;
    }
#endif

#if defined (ENGINE_MATERIAL_ROUGHNESS_TEXTURE)
    float getMaterialRoughness(vec2 uv) {
        return texture(material_roughness_texture, uv).r;
    }
#endif

#if defined (ENGINE_MATERIAL_REFLECTANCE)
    float getMaterialReflectance() {
        return material_reflectance;
    }
#endif

#if defined (ENGINE_MATERIAL_TRANSMISSION)
    float getMaterialTransmission() {
        return material_transmission;
    }
#endif

uint getMaterialShadingModel() {
    return material_shading_model;
}
