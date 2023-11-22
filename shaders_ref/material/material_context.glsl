#include "./material.glsl"

struct MaterialContext {
#if defined (ENGINE_MATERIAL_COLOR)
    vec4 color;
#endif

#if defined (ENGINE_MATERIAL_EMISSIVE_COLOR)
    vec3 emissive_color;
#endif

#if defined (ENGINE_MATERIAL_DIFFUSE_TEXTURE)
    vec4 diffuse;
#endif

#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE)
    vec3 normal;
#endif

#if defined (ENGINE_MATERIAL_EMISSIVEMASK_TEXTURE)
    vec3 emissive_mask;
#endif

#if defined (ENGINE_MATERIAL_BLENDMASK_TEXTURE)
    float blend_mask;
#endif

    float metallic;
    float roughness;

#if defined (ENGINE_MATERIAL_AMBIENT_OCCLUSION_TEXTURE) || defined (ENGINE_MATERIAL_ORM_TEXTURE)
    float ao;
#endif

#if defined (ENGINE_MATERIAL_TESSELLATION_FACTOR)
    vec2 tessellation_factor;
#endif

#if defined (ENGINE_MATERIAL_REFRACTION)
    float IoR;
    float absorption;
#endif

    uint shading_model;
};

MaterialContext computeDefaultMaterialContext() {
    MaterialContext mctx;

#if defined (ENGINE_MATERIAL_COLOR)
    mctx.color = getMaterialColor();
#endif

#if defined (ENGINE_MATERIAL_EMISSIVE_COLOR)
    mctx.emissive_color = getMaterialEmissiveColor();
#endif

#if defined (ENGINE_MATERIAL_DIFFUSE_TEXTURE)
    mctx.diffuse = getMaterialDiffuse(getVertexUV());
#endif

#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE)
    mctx.normal = getMaterialNormal(getVertexUV());
#endif

#if defined (ENGINE_MATERIAL_EMISSIVEMASK_TEXTURE)
    mctx.emissive_mask = getMaterialEmissiveMask(getVertexUV());
#endif

#if defined (ENGINE_MATERIAL_BLENDMASK_TEXTURE)
    mctx.blend_mask = getMaterialBlendMask(getVertexUV());
#endif

#if defined (ENGINE_MATERIAL_ORM_TEXTURE)
    vec3 _orm = getMaterialORM(getVertexUV());
#endif

#if defined (ENGINE_MATERIAL_ORM_TEXTURE)
    mctx.metallic = _orm.b;
#elif defined (ENGINE_MATERIAL_METALLIC_TEXTURE)
    mctx.metallic = getMaterialMetallic(getVertexUV());
#elif defined (ENGINE_MATERIAL_METALLIC)
    mctx.metallic = getMaterialMetallic();
#else
    mctx.metallic = 0.1;
#endif

#if defined (ENGINE_MATERIAL_ORM_TEXTURE)
    mctx.roughness = _orm.g;
#elif defined (ENGINE_MATERIAL_ROUGHNESS_TEXTURE)
    mctx.roughness = getMaterialRoughness(getVertexUV());
#elif defined (ENGINE_MATERIAL_ROUGHNESS)
    mctx.roughness = getMaterialRoughness();
#else
    mctx.roughness = 0.8;
#endif

#if defined (ENGINE_MATERIAL_ORM_TEXTURE)
    mctx.ao = _orm.r;
#elif defined (ENGINE_MATERIAL_AMBIENT_OCCLUSION_TEXTURE)
    mctx.ao = getMaterialAmbientOcclusion(getVertexUV());
#endif

#if defined (ENGINE_MATERIAL_TESSELLATION_FACTOR)
    mctx.tessellation_factor = getMaterialTesselationFactor();
#endif

#if defined (ENGINE_MATERIAL_REFRACTION)
#if defined (ENGINE_MATERIAL_IOR)
    mctx.IoR = getMaterialIOR();
#else
    mctx.IoR = 1.0;
#endif

#if defined (ENGINE_MATERIAL_ABSORPTION)
    mctx.absorption = getMaterialAbsorption();
#else
    mctx.absorption = 0.0;
#endif
#endif

    mctx.shading_model = getMaterialShadingModel();

    return mctx;
}

void customMaterialContext(inout MaterialContext mctx) {
    ENGINE_MATERIAL_FRAGMENT_SNIPPET
}

MaterialContext computeMaterialContext() {
    MaterialContext mctx = computeDefaultMaterialContext();
    customMaterialContext(mctx);
    return mctx;
}

vec4 computeMaterialColor(const MaterialContext mctx) {
    vec4 color = vec4(1.0);

#if defined (ENGINE_MATERIAL_COLOR)
    color *= mctx.color;
#endif

#if defined (ENGINE_MATERIAL_DIFFUSE_TEXTURE)
    color *= mctx.diffuse;
#endif

#if defined (ENGINE_MATERIAL_BLENDMASK_TEXTURE)
    if (mctx.blend_mask <= 0.0) {
        discard;
    }
#endif

#if defined (ENGINE_MATERIAL_EFFECT_MODEL) && defined (BeamEmitter) && defined (BeamSpeed_MODULE)
    if (distance(getVertexPosition(), getParticleStart()) / distance(getParticleStart(), getParticleEnd()) >= getParticleLength()) {
        discard;
    }
#endif

#if defined (ENGINE_MATERIAL_EFFECT_MODEL) && defined (InitialColor_MODULE)
    color *= getParticleColor();
#endif

    return color;
}

vec3 computeMaterialEmissiveColor(const MaterialContext mctx) {
#if !defined (ENGINE_MATERIAL_EMISSIVE_COLOR) && !defined (ENGINE_MATERIAL_EMISSIVEMASK_TEXTURE)
    return vec3(0.0);
#else
    vec3 emissive = vec3(1.0);

#if defined (ENGINE_MATERIAL_EMISSIVE_COLOR)
    emissive *= mctx.emissive_color;
#endif

#if defined (ENGINE_MATERIAL_EMISSIVEMASK_TEXTURE)
    emissive *= mctx.emissive_mask;
#endif

    return emissive;
#endif
}

vec3 computeMaterialNormal(const MaterialContext mctx) {
#if defined (SpriteEmitter) || defined (BeamEmitter)
    return getCameraPosition() - getVertexPosition();
#else
#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
    vec3 normal = mctx.normal;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(getVertexTBN() * normal);
#else
    vec3 normal = normalize(getVertexNormal());
#endif
    return normal;
#endif
}

float computeMaterialAO(const MaterialContext mctx) {
    float ao = 1.0;

#if defined (ENGINE_MATERIAL_ORM_TEXTURE) || defined (ENGINE_MATERIAL_AMBIENT_OCCLUSION_TEXTURE)
    ao *= mctx.ao;
#endif

    return ao;
}