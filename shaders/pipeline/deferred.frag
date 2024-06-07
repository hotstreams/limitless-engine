ENGINE::COMMON

in vec2 uv;

#include "../shading/shading_gctx.glsl"

out vec3 color;

void main() {
    GBufferContext gctx = computeGBufferContext(uv);
    color = shadeFragment(gctx);
}