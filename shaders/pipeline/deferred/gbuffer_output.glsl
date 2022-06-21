// UNSIGNED NORMALIZED [0; 1]
// RGBA8 - RGB - base color, A - ao

// SIGNED NORMALIZED [-1; 1]
// RGB - normal

// UNSIGNED NORMALIZED [0; 1]
// R - roughness, G - metallic, B - shading model (uint)

// FLOATING POINT
// RGB - emissive

layout (location = 0) out vec4 g_base;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec3 g_props;
layout (location = 3) out vec3 g_emissive;
