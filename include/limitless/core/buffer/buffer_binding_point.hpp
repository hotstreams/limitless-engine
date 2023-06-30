#pragma once

#include <limitless/core/buffer/buffer.hpp>

namespace Limitless {
    struct BindingPoint {
        Buffer::Type target;
        GLuint point;
    };
    bool operator<(const BindingPoint& a, const BindingPoint& b) noexcept;
}