#if defined (ENGINE_SETTINGS_SSAO)
    uniform sampler2D _ssao_texture;
    /// 0 = textureLod upscale (texture LINEAR); >0 = Filament-style depth-aware bilinear via texelFetch (no runtime filter change).
    uniform float _ssao_sampling_quality_edge;

    float unpack_ssao_packed_depth(highp vec2 enc) {
        return (enc.x * (256.0 / 257.0) + enc.y * (1.0 / 257.0));
    }

    float getSSAO(vec2 uv, vec3 world_position) {
        if (_ssao_sampling_quality_edge > 0.0) {
            highp vec2 size = vec2(textureSize(_ssao_texture, 0));
            highp vec2 st = uv * size - vec2(0.5);
            ivec2 i00 = ivec2(floor(st));
            highp vec2 f = st - vec2(i00);
            ivec2 mx = max(ivec2(size) - ivec2(1), ivec2(0));

            ivec2 c01 = clamp(i00 + ivec2(0, 1), ivec2(0), mx);
            ivec2 c11 = clamp(i00 + ivec2(1, 1), ivec2(0), mx);
            ivec2 c10 = clamp(i00 + ivec2(1, 0), ivec2(0), mx);
            ivec2 c00 = clamp(i00, ivec2(0), mx);

            vec3 s01 = texelFetch(_ssao_texture, c01, 0).rgb;
            vec3 s11 = texelFetch(_ssao_texture, c11, 0).rgb;
            vec3 s10 = texelFetch(_ssao_texture, c10, 0).rgb;
            vec3 s00 = texelFetch(_ssao_texture, c00, 0).rgb;

            vec4 ao = vec4(s01.r, s11.r, s10.r, s00.r);
            vec4 dg = vec4(s01.g, s11.g, s10.g, s00.g);
            vec4 db = vec4(s01.b, s11.b, s10.b, s00.b);

            highp vec4 depths;
            depths.x = -(unpack_ssao_packed_depth(vec2(dg.x, db.x)) * getCameraFarPlane());
            depths.y = -(unpack_ssao_packed_depth(vec2(dg.y, db.y)) * getCameraFarPlane());
            depths.z = -(unpack_ssao_packed_depth(vec2(dg.z, db.z)) * getCameraFarPlane());
            depths.w = -(unpack_ssao_packed_depth(vec2(dg.w, db.w)) * getCameraFarPlane());

            vec4 b;
            b.x = (1.0 - f.x) * f.y;
            b.y = f.x * f.y;
            b.z = f.x * (1.0 - f.y);
            b.w = (1.0 - f.x) * (1.0 - f.y);

            highp float d = (getView() * vec4(world_position, 1.0)).z;

            highp vec4 w = (vec4(d) - depths) * _ssao_sampling_quality_edge;
            w = max(vec4(1e-4), 1.0 - w * w) * b;
            w = w / (w.x + w.y + w.z + w.w);
            return dot(ao, w);
        }

        return textureLod(_ssao_texture, uv, 0.0).r;
    }
#endif
