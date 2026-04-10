#pragma once

namespace Limitless
{
    enum class LodTransition {
        None,
        /// Screen-space dither + discard; draw two LODs with complementary masks (Unity Cross Fade style).
        CrossFadeDither
    };
}