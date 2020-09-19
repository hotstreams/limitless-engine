#pragma once

#include <context_initializer.hpp>
#include <context_state.hpp>

namespace GraphicsEngine {
    enum class CursorMode { Normal = GLFW_CURSOR_NORMAL, Hidden = GLFW_CURSOR_HIDDEN, Disabled = GLFW_CURSOR_DISABLED };

    enum class WindowHint {
        Resizable = GLFW_RESIZABLE,
        Visible = GLFW_VISIBLE,
        Decorated = GLFW_DECORATED,
        Focused = GLFW_FOCUSED,
        AutoIconify = GLFW_AUTO_ICONIFY,
        Maximized = GLFW_MAXIMIZED,
        Samples = GLFW_SAMPLES
    };
    using WindowHints = std::vector<std::pair<WindowHint, int>>;

    class Context : public ContextInitializer, public ContextState {
    protected:
        GLFWwindow* window {nullptr};
        std::optional<GLFWmonitor*> monitor;
        glm::uvec2 size;

        Context();
        friend void swap(Context& lhs, Context& rhs) noexcept;
    public:
        Context(std::string_view title, glm::uvec2 size, const WindowHints& hints = WindowHints{});
        Context(std::string_view title, glm::uvec2 size, const Context& shared, const WindowHints& hints = WindowHints{});
        ~Context() override;

        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;

        Context(Context&&) noexcept;
        Context& operator=(Context&&) noexcept;

        operator GLFWwindow*() const noexcept;

        void makeCurrent() const noexcept;
        void swapBuffers() const noexcept;
        void pollEvents() const noexcept;

        void setSwapInterval(GLuint interval) const noexcept;
        void setWindowShouldClose(bool value) const noexcept;
        void setWindowIcon(GLFWimage icon) const noexcept;
        void setCursorMode(CursorMode mode) const noexcept;
        void setCursor(GLFWimage cursor) const;
        void setWindowSize(glm::uvec2 size) noexcept;
        void setAspectRatio(glm::uvec2 ratio) noexcept;
        void setFullscreen(bool value) noexcept;
        void setWindowPos(glm::uvec2 pos) const noexcept;
        bool shouldClose() const noexcept;

        void iconify() const noexcept;
        void restore() const noexcept;

        void hide() const noexcept;
        void show() const noexcept;

        void focus() const noexcept;

        [[nodiscard]] glm::uvec2 getSize() const noexcept;
        [[nodiscard]] glm::vec2 getCursorPos() const noexcept;
        [[nodiscard]] bool isFocused() const noexcept;
    };

    void swap(Context& lhs, Context& rhs) noexcept;
}
