Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::ModelType
Limitless::EmitterType

#include "../interface_block/fragment.glsl"

#include "../scene.glsl"
#include "../shading/depth_discarder.glsl"

void main() {
    discardForDepth();
}
