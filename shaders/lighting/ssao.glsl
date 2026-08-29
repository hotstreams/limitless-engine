#if defined (ENGINE_SETTINGS_SSAO)
    uniform sampler2D ssao_texture;

    float getSSAO(vec2 uv) {
        return texture(ssao_texture, uv).r;
    }
#endif
