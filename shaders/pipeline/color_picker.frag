ENGINE::COMMON
ENGINE::MATERIALDEPENDENT

#include "../interface_block/fragment.glsl"
#include "../shading/shading_mctx.glsl"

out vec3 color;

uniform vec3 color_id;

// INSTANCED MODEL
#if defined (ENGINE_MATERIAL_INSTANCED_MODEL)
    layout (std430) buffer color_buffer {
        vec3 _colors[];
    };
#endif
//

vec3 getColorId() {
    #if defined (ENGINE_MATERIAL_INSTANCED_MODEL)
        return _colors[getInstanceId()];
    #else
        return color_id;
    #endif
}

void main() {
    MaterialContext mctx = computeMaterialContext();
    shadeFragment(mctx);

    color = getColorId();
}



