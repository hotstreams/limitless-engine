#if defined (SCREEN_SPACE_AMBIENT_OCCLUSION)
    uniform sampler2D ssao_texture;
#endif

float getSSAO(vec2 uv) {
    #if defined (SCREEN_SPACE_AMBIENT_OCCLUSION)
        return texture(ssao_texture, uv).r;
    #else
        return 1.0;
    #endif
}
