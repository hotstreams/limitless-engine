Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::ModelType
Limitless::EmitterType

#include "../interface_block/fragment.glsl"
#include "../scene.glsl"
#include "../shading/depth_discarder.glsl"

out vec3 color;

uniform vec3 color_id;

void main() {
    discardForDepth();

    color = color_id;
}



