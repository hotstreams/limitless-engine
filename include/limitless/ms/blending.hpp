#pragma once

#include <cstdint>

namespace Limitless {
    class ContextState;
}

namespace Limitless::ms {
    /**
     * Describes how to mix different colors of surfaces together
     */
    enum class Blending {
        /**
         * Opaque material
         */
        Opaque,

        /**
         * Mix based on alpha channel
         *
         * color.rgb * color.a + background.rgb * (1 - color.a)
         */
        Translucent,

        /**
         * Mix colors with addition
         *
         * color.rgb + background.rgb
         */
        Additive,

        /**
         * Mix colors with multiplication
         *
         * color.rgb * background.rgb
         */
        Modulate,

        /**
         * Used for text rendering
         */
        Text
    };

    /**
     * Sets context functions to match specified blending state to the current state
     *
     * @param blending - blending mode
     */
    void setBlendingMode(Blending blending) noexcept;
}
