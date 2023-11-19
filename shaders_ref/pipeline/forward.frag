ENGINE::MATERIALDEPENDENT

#include "../interface_block/fragment.glsl"
#include "../scene.glsl"
#include "../shading/fragment.glsl"

out vec4 color;

void main() {
    MaterialContext mctx = computeMaterialContext();



    FragmentData data = initializeFragmentData();

    customFragment(data);



    color = computeFragment(data);
}