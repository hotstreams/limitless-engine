Limitless::GLSL_VERSION
Limitless::Extensions
Limitless::Settings
Limitless::MaterialType
Limitless::ModelType
Limitless::EmitterType

#include "../interface_block/fragment.glsl"
#include "../scene.glsl"
#include "../shading/fragment.glsl"

#include "./gbuffer_output.glsl"

void main() {
    FragmentData data = initializeFragmentData();

    customFragment(data);

    g_base.rgb = getFragmentBaseColor(data).rgb;
    g_base.a = getFragmentAO(data);

    g_normal = getFragmentNormal(data);

    g_props.r = data.roughness;
    g_props.g = data.metallic;
    g_props.b = float(data.shading_model) / 255.0;

    g_emissive = getFragmentEmissive(data);
}
