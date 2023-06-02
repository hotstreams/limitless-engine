#pragma once

#include <limitless/core/context_debug.hpp>

namespace Limitless {
    enum class CullFace {
        Front = GL_FRONT,
        Back = GL_BACK,
        FrontBack = GL_FRONT_AND_BACK
    };

    enum class FrontFace {
        CW = GL_CW,
        CCW = GL_CCW
    };
}
