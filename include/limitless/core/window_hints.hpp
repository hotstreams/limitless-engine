#pragma once

#include <limitless/core/context_debug.hpp>

#include <vector>

namespace Limitless {
    class WindowHint {
    public:
        enum class Hint {
            Resizable = GLFW_RESIZABLE,
            Visible = GLFW_VISIBLE,
            Decorated = GLFW_DECORATED,
            Focused = GLFW_FOCUSED,
            AutoIconify = GLFW_AUTO_ICONIFY,
            Maximized = GLFW_MAXIMIZED,
            Samples = GLFW_SAMPLES
        };

        Hint hint;
        uint32_t value;
    };

    using WindowHints = std::vector<WindowHint>;
}