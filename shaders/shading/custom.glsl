#if defined (ENGINE_MATERIAL_SHADING_CUSTOM_MODEL)
    vec3 customShading(const ShadingContext sctx, const LightingContext lctx, const Light light) {
        ENGINE_MATERIAL_SHADING_CUSTOM_SNIPPET
    }
#endif
