#pragma once

namespace Limitless::ms {
    // the order should be the same as material.glsl
    enum class Property {
        // vec4
        Color,
        EmissiveColor,

        // texture
        Diffuse,
        Normal,
        Specular,
        EmissiveMask,
        BlendMask,
        MetallicTexture,
        RoughnessTexture,

        // tesselation parameters
        Displacement,
        TessellationFactor,

        // float
        Shininess,
        Metallic,
        Roughness,
    };
}