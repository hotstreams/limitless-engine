ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

// Debug visualization modes
// #define DEBUG_VIEW_MODE 1  // Vertex Normals (geometry)
// #define DEBUG_VIEW_MODE 2  // Tangents
// #define DEBUG_VIEW_MODE 3  // Bitangents  
// #define DEBUG_VIEW_MODE 4  // Normal Map (raw texture)
// #define DEBUG_VIEW_MODE 5  // Final Normals (after TBN)
// #define DEBUG_VIEW_MODE 6  // UV Coordinates
// #define DEBUG_VIEW_MODE 7  // UV Derivatives (mipmap issues - RED = artifacts!)
// #define DEBUG_VIEW_MODE 8  // View Angle (RED = grazing, GREEN = perpendicular)
// #define DEBUG_VIEW_MODE 9  // Tangent Length (GREEN = good, RED = bad)

#include "../interface_block/fragment.glsl"
#include "../instance/instance_fs.glsl"
#include "./scene.glsl"
#include "../material/material_context.glsl"

layout (location = 0) out vec3 albedo;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 properties;
layout (location = 3) out vec3 emissive;
layout (location = 4) out vec3 info;
layout (location = 5) out vec4 outline;

void main() {
    MaterialContext mctx = computeMaterialContext();

    vec3 computed_normal = computeMaterialNormal(mctx);

#if defined(DEBUG_VIEW_MODE)
    // Debug visualization active - output debug view
    vec3 debug_color = debugVisualize(mctx, computed_normal);
    albedo = debug_color;
    normal = vec3(0.0, 0.0, 1.0); // Flat normal for debug
    emissive = vec3(0.0);
#else
    // Normal rendering
    albedo = computeMaterialColor(mctx).rgb;
    normal = computed_normal;
    emissive = computeMaterialEmissiveColor(mctx);
#endif

    properties.r = mctx.roughness;
    properties.g = mctx.metallic;
    properties.b = computeMaterialAO(mctx);

    info.r = float(mctx.shading_model) / 255.0;
    info.g = float(getDecalMask()) / 255.0;
    info.b = 0.0;

    outline.rgb = getOutlineColor();
    outline.a = getIsOutlined() == 1u ? float(getId()) / 65535.0 : 0.0;
}
