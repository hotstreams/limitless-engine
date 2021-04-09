Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::ModelType

#include "../glsl/scene_lighting.glsl"
#include "../glsl/material.glsl"

out vec4 color;

in vec3 fs_uv;

void main() {
    vec3 uv = fs_uv;

    #include "../glsl/material_variables.glsl"

    Limitless::CustomMaterialFragmentCode

    // computing final color
    vec4 fragment_color = vec4(1.0);
    #include "../glsl/material_computation.glsl"

    color = fragment_color;
}