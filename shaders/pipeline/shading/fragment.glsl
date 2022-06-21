#include "../material/material.glsl"
#include "./fragment_data.glsl"
#include "../material/custom_fragment.glsl"
#include "./fragment_data_initializer.glsl"

vec4 getFragmentBaseColor(const FragmentData data) {
    vec4 baseColor = vec4(1.0);

#if defined (MATERIAL_COLOR)
    baseColor *= data.color;
#endif

#if defined (MATERIAL_DIFFUSE)
    baseColor *= data.baseColor;
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
    baseColor *= getParticleColor();
#endif

    return baseColor;
}

vec3 getFragmentNormal(const FragmentData data) {
#if defined (SpriteEmitter) || defined (BeamEmitter)
    return getCameraPosition() - getVertexPosition();
#else
    #if defined (MATERIAL_NORMAL) && defined (NORMAL_MAPPING)
        vec3 normal = data.normal;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(getVertexTBN() * normal);
    #else
        vec3 normal = normalize(getVertexNormal());
    #endif

    return normal;
#endif
}

#if defined (SCREEN_SPACE_AMBIENT_OCCLUSION)
    uniform sampler2D ssao_texture;
#endif

float getFragmentAO(const FragmentData data) {
    float ao = 1.0;

#if defined (MATERIAL_AMBIENT_OCCLUSION_TEXTURE)
    ao = data.ao;
#endif

#if defined (SCREEN_SPACE_AMBIENT_OCCLUSION)
    ao = min(ao, texture(ssao_texture, uv).r);
#endif

    return ao;
}

vec3 getFragmentEmissive(const FragmentData data) {
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

#include "./shading.glsl"

#if defined (MATERIAL_REFRACTION)
    #include "./refraction.glsl"
#endif

vec4 computeFragment(const FragmentData data) {
    vec4 baseColor = getFragmentBaseColor(data);
    vec3 normal = getFragmentNormal(data);
    vec3 emissive = getFragmentEmissive(data);
    float ao = getFragmentAO(data);

    vec4 color;
    color.rgb = getFragmentColor(baseColor.rgb, ao, normal, getVertexPosition(), data.metallic, data.roughness, data.shading_model);
    color.a = baseColor.a;

    #if defined (MATERIAL_REFRACTION)
        color.rgb += computeRefraction(normal, data.metallic, baseColor.rgb, data.IoR, data.absorption);
    #endif

    color.rgb += getFragmentEmissive(data);

    return color;
}
