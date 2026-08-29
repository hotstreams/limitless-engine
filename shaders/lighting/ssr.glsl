#if defined (ENGINE_SETTINGS_SSR)
    uniform sampler2D ssr_texture;
    uniform float ssr_strength;

    vec3 getSSR(vec2 uv) {
        return texture(ssr_texture, uv).rgb * ssr_strength;
    }
#endif
