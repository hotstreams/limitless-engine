#if defined (ENGINE_SETTINGS_SSAO)
    uniform sampler2D _ssao_texture;

    float getSSAO(vec2 uv) {
        return texture(_ssao_texture, uv).r;
    }
#endif
