#if defined (ENGINE_SETTINGS_SSR)
    uniform sampler2D _ssr_texture;
    uniform float _ssr_strength;

    vec3 getSSR(vec2 uv) {
        return texture(_ssr_texture, uv).rgb * _ssr_strength;
    }
#endif
