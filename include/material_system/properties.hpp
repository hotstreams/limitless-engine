#pragma once

namespace GraphicsEngine {
    enum class PropertyType {
        Color,
        EmissiveColor,
        Diffuse,
        Specular,
        Normal,
        Displacement,
        EmissiveMask,
        BlendMask,
        Shininess,
        Metallic,
        MetallicTexture,
        Roughness,
        RoughnessTexture
    };

    enum class Blending { Opaque, Translucent, Additive, Modulate, OpaqueHalf, Text };
    enum class Shading { Lit, Unlit };

    class ContextState;
    void setBlendingMode(ContextState& ctx, Blending blending) noexcept;

    //sets blending mode to the current context
    void setBlendingMode(Blending blending) noexcept;
}