ENGINE::COMMON

#include "../shading/shading_gctx.glsl"

in vec2 uv;

out vec3 color;

//#if defined (SCREEN_SPACE_AMBIENT_OCCLUSION)
//    uniform sampler2D ssao_texture;
//#endif

//TODO: refactor - separate responsibilities

//#ifdef SCREEN_SPACE_REFLECTIONS
//    uniform sampler2D ssr_texture;
//    uniform float _ssr_strength;
//#endif

void main() {
//#if defined (SCREEN_SPACE_AMBIENT_OCCLUSION)
//    base.a *= texture(ssao_texture, uv).r;
//#endif

    const GBufferContext gctx = computeGBufferContext(uv);
    color = shadeFragment(gctx);

//  just add for now for test
//#ifdef SCREEN_SPACE_REFLECTIONS
//    //  TODO: move to indirect lighting
//    color += texture(ssr_texture, uv).rgb *_ssr_strength;
//#endif
}
