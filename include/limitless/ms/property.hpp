#pragma once

namespace Limitless::ms {
    enum class Property {
        // vec4; rgb - color; a - intensity; linear space; [0 - 1]
        Color,
        // vec4; rgb - color; a - intensity; linear space; [0 - n]
        EmissiveColor,
        // texture
        Diffuse,
        // texture
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

        Reflectance,
        Transmission,
        MicroThickness,
        Thickness,

        SubsurfaceColor,
        SubsurfacePower,

        SheenColor
    };
}