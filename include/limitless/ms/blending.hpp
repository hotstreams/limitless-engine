#pragma once

#include <stdint.h>

namespace Limitless {
    class ContextState;
}

namespace Limitless::ms {
    enum class Blending {
        Opaque,
        MultipleOpaque,
        Translucent,
        Additive,
        Modulate,
        Text
    };

    //sets blending mode to the current context
    void setBlendingMode(Blending blending) noexcept;
    void setBlendingMode(ContextState& ctx, Blending blending, uint32_t opaque_count = 1) noexcept;
}