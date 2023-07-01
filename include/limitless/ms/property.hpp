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

        /*
         *      Ambient Occlusion, Roughness, and Metallic
         *
         *      Red: Ambient Occlusion
         *      Green: Roughness
         *      Blue: Metallic
         *
         *      overrides other orm properties
         */
        ORM,

        // vec2
        TessellationFactor,

        // float
        Metallic,
        Roughness,

        IoR,
        Absorption,
    };
}