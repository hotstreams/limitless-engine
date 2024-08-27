ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

#include "../interface_block/fragment.glsl"
#include "../shading/shading_mctx.glsl"
#include "../functions/reconstruct_position.glsl"
#include "../functions/reconstruct_normal.glsl"

layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 properties;
layout (location = 3) out vec4 emissive;
layout (location = 4) out vec4 info;

uniform sampler2D depth_texture;
uniform sampler2D info_texture;

uniform mat4 decal_VP;
uniform uint projection_mask;

uniform float decal_blend = 1.0;

void main() {
    vec2 uv = gl_FragCoord.xy / getResolution();

    // check instance mask
    uint mask = uint(texture(info_texture, uv).g * 255.0);
    if ((mask & projection_mask) == 0u) {
        discard;
    }

    // sample depth from gbuffer
    float depth = texture(depth_texture, uv).r;

    // reconstruct world space position
    vec3 positionWS = reconstructPosition(uv, depth);

    // object space
    vec3 positionOS = (decal_VP * vec4(positionWS, 1.0)).xyz;

    // box check
    const float epsilon = 0.005;
    if (any(greaterThan(abs(positionOS) - vec3(0.5), vec3(epsilon)))) {
        discard;
    }

    // decal uv
    vec2 decal_uv = positionOS.xz + 0.5;

    MaterialContext ctx = computeMaterialContext(decal_uv);

    // albedo
#if defined (ENGINE_MATERIAL_COLOR) || defined (ENGINE_MATERIAL_DIFFUSE_TEXTURE) || defined (ENGINE_MATERIAL_BLENDMASK_TEXTURE)
    albedo = computeMaterialColor(ctx);
    albedo.a = decal_blend;
#endif

    // normal
#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
    mat3 TBN = reconstructTBN(depth_texture, uv, depth, positionWS, vec2(1.0) / getResolution());

    vec3 nn = normalize(ctx.normal * 2.0 - 1.0);
    nn = normalize(TBN * nn);

    normal = vec4(nn, decal_blend);
#endif

    // roughness
#if defined (ENGINE_MATERIAL_ORM_TEXTURE) || defined (ENGINE_MATERIAL_ROUGHNESS_TEXTURE) || defined (ENGINE_MATERIAL_ROUGHNESS)
    properties.r = ctx.roughness;
    properties.a = decal_blend;
#endif

    // metallic
#if defined (ENGINE_MATERIAL_ORM_TEXTURE) || defined (ENGINE_MATERIAL_METALLIC_TEXTURE) || defined (ENGINE_MATERIAL_METALLIC)
    properties.g = ctx.metallic;
    properties.a = decal_blend;
#endif

    // ao
#if defined (ENGINE_MATERIAL_ORM_TEXTURE) || defined (ENGINE_MATERIAL_AMBIENT_OCCLUSION_TEXTURE)
    properties.b = computeMaterialAO(ctx);
    properties.a = decal_blend;
#endif

    // emissive
#if defined (ENGINE_MATERIAL_EMISSIVE_COLOR) || defined (ENGINE_MATERIAL_EMISSIVEMASK_TEXTURE)
    emissive = vec4(computeMaterialEmissiveColor(ctx), decal_blend);
#endif

    // always overrides shading model
    info.r = float(ctx.shading_model) / 255.0;
    info.a = 1.0;
}