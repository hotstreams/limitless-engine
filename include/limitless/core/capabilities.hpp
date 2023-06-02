#pragma once

#include <limitless/core/context_debug.hpp>

namespace Limitless {
    enum class Capabilities {
        DepthTest = GL_DEPTH_TEST,
        Blending = GL_BLEND,
        ProgramPointSize = GL_PROGRAM_POINT_SIZE,
        ScissorTest = GL_SCISSOR_TEST,
        StencilTest = GL_STENCIL_TEST,
        CullFace = GL_CULL_FACE
    };
}