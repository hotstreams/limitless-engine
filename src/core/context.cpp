#include <core/context.hpp>
#include <stb_image.h>

using namespace LimitlessEngine;

Context::Context(std::string_view title, glm::uvec2 s, const WindowHints& hints)
    : ContextInitializer(), ContextState(), size(s) {

    for (const auto& [hint, value] : hints) {
        glfwWindowHint(static_cast<int>(hint), value);
    }

    window = glfwCreateWindow(size.x, size.y, title.data(), nullptr, nullptr);

    if (!window) {
        throw context_error{"Failed to create window"};
    }

    if (!glew_inited) {
        makeCurrent();
        initializeGLEW();
    }

    init();

    defaultHints();

    registerState(window);
}

Context::Context(std::string_view title, glm::uvec2 s, const Context& shared, const WindowHints& hints)
    : ContextInitializer(), ContextState(), size(s) {

    for (const auto& [hint, value] : hints) {
        glfwWindowHint(static_cast<int>(hint), value);
    }

    window = glfwCreateWindow(size.x, size.y, title.data(), nullptr, shared.window);

    if (!window) {
        throw context_error{"Failed to create window"};
    }

    init();

    defaultHints();

    registerState(window);
}

void LimitlessEngine::swap(Context& lhs, Context& rhs) noexcept {
    using std::swap;

    lhs.swapStateMap(lhs, rhs);

    swap(lhs.window, rhs.window);
    swap(lhs.monitor, rhs.monitor);
    swap(lhs.size, rhs.size);
}

Context::Context(Context&& rhs) noexcept : ContextState(std::move(rhs)) {
    swap(*this, rhs);
}

Context& Context::operator=(Context&& rhs) noexcept {
    static_cast<ContextState&>(*this) = std::move(rhs);
    swap(*this, rhs);
    return *this;
}

Context::operator GLFWwindow*() const noexcept {
    return window;
}

Context::~Context() {
    if (window) {
        unregisterState(window);
        glfwDestroyWindow(window);
    }
}

void Context::makeCurrent() const noexcept {
    glfwMakeContextCurrent(window);
}

void Context::swapBuffers() const noexcept {
    glfwSwapBuffers(window);
}

void Context::setSwapInterval(GLuint interval) const noexcept {
    glfwSwapInterval(interval);
}

void Context::setFullscreen(bool value) noexcept {
    if (value) {
        if (monitor) return;

        monitor = glfwGetPrimaryMonitor();
        const auto* mode = glfwGetVideoMode(monitor.value());

        glfwSetWindowMonitor(window, monitor.value(), 0, 0, mode->width, mode->height, mode->refreshRate);
    } else {
        if (!monitor) return;

        const auto* mode = glfwGetVideoMode(monitor.value());
        monitor = std::nullopt;
        glfwSetWindowMonitor(window, monitor.value(), 0, 0, mode->width, mode->height, mode->refreshRate);
    }
}

void Context::pollEvents() const noexcept {
    glfwPollEvents();
}

void Context::setWindowShouldClose(bool value) const noexcept {
    glfwSetWindowShouldClose(window, value);
}

glm::uvec2 Context::getSize() const noexcept {
    return size;
}

glm::vec2 Context::getCursorPos() const noexcept {
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    return { x, y };
}

void Context::setWindowSize(glm::uvec2 s) noexcept {
    size = s;
    glfwSetWindowSize(window, size.x, size.y);
}

bool Context::isFocused() const noexcept {
    return glfwGetWindowAttrib(window, GLFW_FOCUSED);
}

void Context::setWindowIcon(GLFWimage icon) const noexcept {
    glfwSetWindowIcon(window, 1, &icon);
    stbi_image_free(icon.pixels);
}

bool Context::shouldClose() const noexcept {
    return glfwWindowShouldClose(window);
}

void Context::setAspectRatio(glm::uvec2 ratio) noexcept {
    glfwSetWindowAspectRatio(window, ratio.x, ratio.y);
}

void Context::setWindowPos(glm::uvec2 pos) const noexcept {
    glfwSetWindowPos(window, pos.x, pos.y);
}

void Context::iconify() const noexcept {
    glfwIconifyWindow(window);
}

void Context::restore() const noexcept {
    glfwRestoreWindow(window);
}

void Context::hide() const noexcept {
    glfwHideWindow(window);
}

void Context::show() const noexcept {
    glfwShowWindow(window);
}

void Context::focus() const noexcept {
    glfwFocusWindow(window);
}

void Context::setCursorMode(CursorMode mode) const noexcept {
    glfwSetInputMode(window, GLFW_CURSOR, static_cast<int>(mode));
}

void Context::setCursor(GLFWimage image) const {
    auto* cursor = glfwCreateCursor(&image, 0, 0);

    glfwSetCursor(window, cursor);
}
