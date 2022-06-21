#pragma once

namespace Limitless::ms {
    enum class Property {
        // vec4
        Color,
        EmissiveColor,

        // texture
        Diffuse,
        Normal,
        EmissiveMask,
        BlendMask,
        MetallicTexture,
        RoughnessTexture,
        AmbientOcclusionTexture,

        // vec2
        TessellationFactor,

        // float
        Metallic,
        Roughness,

        IoR,
        Absorption,
    };
}