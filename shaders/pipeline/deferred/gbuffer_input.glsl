// UNSIGNED NORMALIZED [0; 1]
// RGBA8 - RGB - base color, A - ao

// SIGNED NORMALIZED [-1; 1]
// RGB - normal

// UNSIGNED NORMALIZED [0; 1]
// R - roughness, G - metallic, B - shading model (uint)

// FLOATING POINT
// RGB - emissive

uniform sampler2D base_texture;
uniform sampler2D normal_texture;
uniform sampler2D props_texture;
uniform sampler2D depth_texture;
uniform sampler2D emissive_texture;