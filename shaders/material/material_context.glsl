// For indirect rendering, use material_indirect.glsl instead of material.glsl
// It defines SSBO-based material accessors that override the uniform-based ones
#if defined(ENGINE_MATERIAL_INDIRECT_MODEL)
#include "./material_indirect.glsl"
#else
#include "./material.glsl"
#endif

#include "../functions/speedtree_billboard_atlas.glsl"

struct MaterialContext {
    // vertex parameters
//    vec3 vertex_position;
    vec3 vertex_normal;

#if defined (ENGINE_MATERIAL_COLOR)
    vec4 color;
#endif

#if defined (ENGINE_MATERIAL_EMISSIVE_COLOR)
    vec3 emissive_color;
#endif

#if defined (ENGINE_MATERIAL_DIFFUSE_TEXTURE)
    vec4 diffuse;
#endif

#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) || defined(ENGINE_MATERIAL_NORMAL_MAP)
    vec3 normal;
    vec4 tangent; // xyz=tangent, w=handedness (bitangent sign)
#endif

#if defined (ENGINE_MATERIAL_EMISSIVEMASK_TEXTURE)
    vec3 emissive_mask;
#endif

#if defined (ENGINE_MATERIAL_BLENDMASK_TEXTURE)
    float blend_mask;
#endif

    float metallic;
    float roughness;

#if defined (ENGINE_MATERIAL_AMBIENT_OCCLUSION_TEXTURE) || defined (ENGINE_MATERIAL_ORM_TEXTURE) || defined(ENGINE_MATERIAL_ORM_MAP)
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

MaterialContext computeDefaultMaterialContext(vec2 uv) {
    MaterialContext mctx;

//    mctx.vertex_position = getVertexPosition();

#if defined (ENGINE_VERTEX_NORMAL)
    mctx.vertex_normal = getVertexNormal();
#endif

#if defined (ENGINE_MATERIAL_COLOR)
    mctx.color = getMaterialColor();
#endif

#if defined (ENGINE_MATERIAL_EMISSIVE_COLOR)
    mctx.emissive_color = getMaterialEmissiveColor();
#endif

#if defined (ENGINE_MATERIAL_DIFFUSE_TEXTURE)
    mctx.diffuse = getMaterialDiffuse(uv);
#endif

#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE)
    mctx.normal = getMaterialNormal(uv);
    mctx.tangent = getVertexTangent();
#endif

#if defined (ENGINE_MATERIAL_EMISSIVEMASK_TEXTURE)
    mctx.emissive_mask = getMaterialEmissiveMask(uv);
#endif

#if defined (ENGINE_MATERIAL_BLENDMASK_TEXTURE)
    mctx.blend_mask = getMaterialBlendMask(uv);
#endif

#if defined (ENGINE_MATERIAL_ORM_TEXTURE)
    vec3 _orm = getMaterialORM(uv);
#endif

#if defined (ENGINE_MATERIAL_ORM_TEXTURE)
    mctx.metallic = _orm.b;
#elif defined (ENGINE_MATERIAL_METALLIC_TEXTURE)
    mctx.metallic = getMaterialMetallic(uv);
#elif defined (ENGINE_MATERIAL_METALLIC)
    mctx.metallic = getMaterialMetallic();
#else
    mctx.metallic = 0.1;
#endif

#if defined (ENGINE_MATERIAL_ORM_TEXTURE)
    mctx.roughness = _orm.g;
#elif defined (ENGINE_MATERIAL_ROUGHNESS_TEXTURE)
    mctx.roughness = getMaterialRoughness(uv);
#elif defined (ENGINE_MATERIAL_ROUGHNESS)
    mctx.roughness = getMaterialRoughness();
#else
    mctx.roughness = 0.8;
#endif

#if defined (ENGINE_MATERIAL_ORM_TEXTURE)
    mctx.ao = _orm.r;
#elif defined (ENGINE_MATERIAL_AMBIENT_OCCLUSION_TEXTURE)
    mctx.ao = getMaterialAmbientOcclusion(uv);
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

ENGINE_MATERIAL_GLOBAL_FRAGMENT_DEFINITIONS

void customMaterialContext(inout MaterialContext mctx, const VertexContext vctx) {
    ENGINE_MATERIAL_FRAGMENT_SNIPPET
}

MaterialContext computeMaterialContext(VertexContext vctx) {
    #if defined (ENGINE_MATERIAL_INDIRECT_MODEL)
        // Set draw ID for indirect material access
        setIndirectDrawId(vctx.draw_id);
    #endif

    #if defined (ENGINE_MATERIAL_DEFAULT_COMPUTATION) && defined (ENGINE_VERTEX_UV)
#if defined (ENGINE_MATERIAL_BILLBOARD_MODE)
        vec2 sampling_uv = vctx.uv;
        if (getMaterialBillboardMode() == 4u) {
            InstanceContext ictx_atlas = computeInstanceContext(vctx);
            sampling_uv = speedtree_bb_atlas_uv(vctx.uv, ictx_atlas.model_matrix);
        }
        MaterialContext mctx = computeDefaultMaterialContext(sampling_uv);
#else
        MaterialContext mctx = computeDefaultMaterialContext(vctx.uv);
#endif
    #else
        MaterialContext mctx;
    #endif

    customMaterialContext(mctx, vctx);

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

#if (defined(ENGINE_MATERIAL_NORMAL_TEXTURE) || defined(ENGINE_MATERIAL_NORMAL_MAP)) && defined(ENGINE_SETTINGS_NORMAL_MAPPING)
    vec3 normal = mctx.normal;

#if defined (ENGINE_MATERIAL_TWO_SIDED)
    normal = gl_FrontFacing ? normal : -normal;
#endif
    normal = normalize(normal * 2.0 - 1.0);

    vec3 N = normalize(mctx.vertex_normal);
    vec3 T = normalize(mctx.tangent.xyz);

    //T = normalize(T - dot(T, N) * N);
    float ts = (mctx.tangent.w < 0.0) ? -1.0 : 1.0;
    vec3 B = normalize(cross(N, T)) * ts;

    // Tangent-space to world-space normal mapping.
    // NOTE: We currently don't propagate tangent handedness into the fragment stage, so B is cross(N,T).
    mat3 TBN = mat3(T, B, N);

#if defined (ENGINE_MATERIAL_TWO_SIDED)
    // Keep shading consistent for backfaces by flipping the basis.
    if (!gl_FrontFacing) {
        TBN[0] = -TBN[0];
        TBN[1] = -TBN[1];
        TBN[2] = -TBN[2];
    }
#endif

    return normalize(TBN * normal);
#else
    vec3 normal = normalize(mctx.vertex_normal);
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