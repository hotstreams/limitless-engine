Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::ModelType
Limitless::EmitterType

#include "../interface_block/fragment.glsl"
#include "../scene.glsl"
#include "../shading/fragment.glsl"

out vec4 color;

void main() {
    FragmentData data = initializeFragmentData();

    customFragment(data);

    color = computeFragment(data);
}