#pragma once

#include <limitless/core/context_initializer.hpp>
#include <functional>
#include <glm/glm.hpp>

namespace Limitless {
    enum class MouseButton : uint8_t {
        Left = GLFW_MOUSE_BUTTON_LEFT,
        Right = GLFW_MOUSE_BUTTON_RIGHT,
        Middle = GLFW_MOUSE_BUTTON_MIDDLE
    };

    enum class InputState : uint8_t {
        Released = GLFW_RELEASE,
        Pressed = GLFW_PRESS,
        Repeat = GLFW_REPEAT
    };

    enum class Modifier : uint8_t {
        Shift = 1,
        Control = 2,
        Alt = 4,
        Super = 8,
        CapsLock = 16,
        NumLock = 32
    };

    using FramebufferCallback = std::function<void(glm::uvec2 size)>;
    using MouseClickCallback = std::function<void(glm::dvec2 pos, MouseButton button, InputState state, Modifier modifier)>;
    using MouseMoveCallback = std::function<void(glm::dvec2 pos)>;
    using KeyCallback = std::function<void(int key, int scancode, InputState state, Modifier modifier)>;
    using CharCallback = std::function<void(uint32_t utf8)>;
    using ScrollCallback = std::function<void(glm::dvec2 pos)>;
}
