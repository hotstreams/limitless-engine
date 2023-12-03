ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

#include "../interface_block/fragment.glsl"
#include "../shading/shading_mctx.glsl"
#include "../functions/reconstruct_position.glsl"
#include "../functions/reconstruct_normal.glsl"

layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec3 properties;
layout (location = 3) out vec4 emissive;
layout (location = 4) out vec4 info;

uniform sampler2D depth_texture;
uniform mat4 decal_VP;

in vec3 clip;

void main() {
    // perpspective division to get ndc
    vec2 ndc = clip.xy / clip.z;

    // map from [-1, 1] to [0, 1]
    vec2 uv = ndc * 0.5 + 0.5;

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

    albedo = computeMaterialColor(ctx);

#if defined (ENGINE_MATERIAL_NORMAL_TEXTURE) && defined (ENGINE_SETTINGS_NORMAL_MAPPING)
    mat3 TBN = reconstructTBN(depth_texture, uv, depth, positionWS, vec2(1.0) / getResolution());

    vec3 nn = normalize(ctx.normal * 2.0 - 1.0);
    nn = normalize(TBN * nn);

    normal = vec4(nn, albedo.a);
#endif

    properties.r = ctx.roughness;
    properties.g = ctx.metallic;
    properties.b = computeMaterialAO(ctx);

    emissive = vec4(computeMaterialEmissiveColor(ctx), albedo.a);

    info.a = float(ctx.shading_model) / 255.0;
}