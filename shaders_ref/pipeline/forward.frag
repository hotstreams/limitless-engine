ENGINE::MATERIALDEPENDENT

#include "../interface_block/fragment.glsl"
#include "../scene.glsl"
#include "../lighting/light.glsl"

out vec4 color;

void main() {
    MaterialContext mctx = computeMaterialContext();
    ShadingContext sctx = computeShadingContext(mctx);
    color = computeLights(sctx);
}