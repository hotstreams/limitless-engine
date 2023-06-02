#pragma once

#include <limitless/core/context_debug.hpp>

namespace Limitless {
    enum class StencilOp {
        Keep = GL_KEEP,
        Zero = GL_ZERO,
        Replace = GL_REPLACE,
        Incr = GL_INCR,
        IncrWrap = GL_INCR_WRAP,
        Decr = GL_DECR,
        DecrWrap = GL_DECR_WRAP,
        Invert = GL_INVERT
    };

    enum class StencilFunc {
        Never = GL_NEVER,
        Less = GL_LESS,
        Lequal = GL_LEQUAL,
        Greater = GL_GREATER,
        Gequal = GL_GEQUAL,
        Nequal = GL_NOTEQUAL,
        Always = GL_ALWAYS
    };
}
