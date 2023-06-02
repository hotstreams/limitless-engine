#pragma once

#include <limitless/core/context_debug.hpp>

namespace Limitless {
    enum class DepthFunc {
        Never = GL_NEVER,
        Less = GL_LESS,
        Equal = GL_EQUAL,
        Lequal = GL_LEQUAL,
        Greater = GL_GREATER,
        Notequal = GL_NOTEQUAL,
        Gequal = GL_GEQUAL,
        Always = GL_ALWAYS
    };

    enum class DepthMask {
        True = GL_TRUE,
        False = GL_FALSE
    };
}
