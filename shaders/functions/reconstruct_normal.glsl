// Accurate view-space normal reconstruction
// Based on Yuwen Wu "Accurate Normal Reconstruction"
// (https://atyuwen.github.io/posts/normal-reconstruction)
//
// depthTexture   : the depth texture in reversed-Z
// uv             : normalized coordinates
// depth          : linear depth at uv
// position       : view space position at uv
// texel          : 1/depth_width, 1/depth_height
// positionParams : invProjection[0][0] * 2, invProjection[1][1] * 2
//

vec3 face_normal(vec3 dpdx, vec3 dpdy) {
    return normalize(cross(dpdx, dpdy));
}

vec3 reconstructViewSpaceNormal(
    const sampler2D depth_texture,
    const vec2 uv,
    const float depth,
    const vec3 position,
    vec2 texel) {

    vec3 pos_c = position;
    vec2 dx = vec2(texel.x, 0.0);
    vec2 dy = vec2(0.0, texel.y);

    vec4 H;
    H.x = texture(depth_texture, uv - dx).r;
    H.y = texture(depth_texture, uv + dx).r;
    H.z = texture(depth_texture, uv - dx * 2.0).r;
    H.w = texture(depth_texture, uv + dx * 2.0).r;
    vec2 he = abs((2 * H.zw - H.xy) - depth);
    vec3 pos_l = reconstructViewSpacePosition(uv - dx, H.x);
    vec3 pos_r = reconstructViewSpacePosition(uv + dx, H.y);
    vec3 dpdx = (he.x < he.y) ? (pos_c - pos_l) : (pos_r - pos_c);

    vec4 V;
    V.x = texture(depth_texture, uv - dy).r;
    V.y = texture(depth_texture, uv + dy).r;
    V.z = texture(depth_texture, uv - dy * 2.0).r;
    V.w = texture(depth_texture, uv + dy * 2.0).r;
    vec2 ve = abs((2 * V.zw - V.xy) - depth);
    vec3 pos_d = reconstructViewSpacePosition(uv - dy, V.x);
    vec3 pos_u = reconstructViewSpacePosition(uv + dy, V.y);
    vec3 dpdy = (ve.x < ve.y) ? (pos_c - pos_d) : (pos_u - pos_c);
    return face_normal(dpdx, dpdy);
}