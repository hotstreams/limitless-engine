#pragma once

#include <limitless/core/context_debug.hpp>

namespace Limitless {
    enum class BlendFactor {
        None,
        Zero = GL_ZERO,
        One = GL_ONE,
        DstColor = GL_DST_COLOR,
        SrcColor = GL_SRC_COLOR,
        SrcAlpha = GL_SRC_ALPHA,
        DstAplha = GL_DST_ALPHA,
        OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
        OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
        BlendColor = GL_CONSTANT_COLOR
    };
}
