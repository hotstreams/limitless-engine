/**
 *  Shades current fragment with defined material model for specified light
 */
vec3 shadeForLight(const ShadingContext sctx, const LightingContext lctx, const Light light) {
#if defined (ENGINE_MATERIAL_SHADING_REGULAR_MODEL)
    return regularShading(sctx, lctx, light);
#elif defined (ENGINE_MATERIAL_SHADING_CLOTH_MODEL)
    return clothShading(sctx, lctx, light);
#elif defined (ENGINE_MATERIAL_SHADING_SUBSURFACE_MODEL)
    return subsurfaceShading(sctx, lctx, light);
#elif defined (ENGINE_MATERIAL_SHADING_CUSTOM_MODEL)
    return customShading(sctx, lctx, light);
#endif
}