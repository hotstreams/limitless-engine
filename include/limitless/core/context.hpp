#pragma once

#include <limitless/core/context_initializer.hpp>
#include <limitless/core/context_state.hpp>
#include <limitless/core/context_observer.hpp>
#include <limitless/core/window_hints.hpp>

#include <stdexcept>
#include <optional>

namespace Limitless {
    enum class CursorMode {
        Normal = GLFW_CURSOR_NORMAL,
        Hidden = GLFW_CURSOR_HIDDEN,
        Disabled = GLFW_CURSOR_DISABLED
    };

    struct context_error : public std::runtime_error {
        explicit context_error(const char* error) noexcept : runtime_error{error} {}
    };

    class Context final : public ContextInitializer, public ContextState {
    private:
        static inline std::unordered_map<GLFWwindow*, Context*> contexts;

        GLFWwindow* window {};

        std::optional<GLFWmonitor*> monitor;
        glm::uvec2 size {1, 1};

        FramebufferCallback framebuffer_callback;
        MouseClickCallback mouseclick_callback;
        MouseMoveCallback mousemove_callback;
        ScrollCallback scroll_callback;
        CharCallback char_callback;
        KeyCallback key_callback;

        void registerCallbacks() noexcept;
        void registerContext() noexcept;
        void unregisterContext() noexcept;

        static void framebufferCallback(GLFWwindow* win, int w, int h);
        static void mouseclickCallback(GLFWwindow* win, int button, int action, int modifiers);
        static void mousemoveCallback(GLFWwindow* win, double x, double y);
        static void keyboardCallback(GLFWwindow* win, int key, int scancode, int action, int modifiers);
        static void scrollCallback(GLFWwindow* win, double x, double y);
        static void charCallback(GLFWwindow* win, uint32_t utf);
    public:
        Context(const std::string& title, glm::uvec2 size, const Context* shared, const WindowHints& hints);

        friend void swap(Context& lhs, Context& rhs) noexcept;
    public:
        ~Context() override;

        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;

        Context(Context&&) noexcept;
        Context& operator=(Context&&) noexcept;

        operator GLFWwindow*() const noexcept;
        GLFWwindow* getWindow() const noexcept;

        void makeCurrent() const noexcept;
        void swapBuffers() const noexcept;
        void pollEvents() const;

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

        void setStickyMouseButtons(bool value) const noexcept;
        void setStickyKeys(bool value) const noexcept;

        InputState getKey(int key) const noexcept;
        bool isPressed(int key) const noexcept;

        InputState getMouseButton(MouseButton button);

        void doOnFramebufferChange(FramebufferCallback callback);
        void doOnMouseClick(MouseClickCallback callback);
        void doOnMouseMove(MouseMoveCallback callback);
        void doOnKeyPress(KeyCallback callback);
        void doOnCharPress(CharCallback callback);
        void doOnScroll(ScrollCallback callback);

        void onFramebufferChange(glm::uvec2 size);
        void onMouseClick(glm::dvec2 pos, MouseButton button, InputState state, Modifier modifier) const;
        void onMouseMove(glm::dvec2 pos) const;
        void onKey(int key, int scancode, InputState state, Modifier modifier) const;
        void onScroll(glm::dvec2 pos) const;
        void onChar(uint32_t utf8) const;

        static Context* getCurrentContext() noexcept;

        /**
         * Invokes specified function for the current context if present
         */
        static void apply(const std::function<void(Context&)>& function);

        class Builder {
        private:
            std::string ctx_title;
            glm::uvec2 ctx_size {640, 480};
            Context* ctx_shared {};
            WindowHints ctx_hints;
            uint32_t ctx_interval {};
            std::function<GLFWimage()> ctx_icon;
            CursorMode ctx_cursor {CursorMode::Normal};
            bool ctx_sticky_keys {false};

            FramebufferCallback framebuffer_callback;
            MouseClickCallback mouseclick_callback;
            MouseMoveCallback mousemove_callback;
            ScrollCallback scroll_callback;
            CharCallback char_callback;
            KeyCallback key_callback;
        public:
            Builder& title(const std::string& title);
            Builder& size(glm::uvec2 size);
            Builder& shared(Context* shared);
            Builder& hints(const WindowHints& hints);

            Builder& resizeable();
            Builder& not_resizeable();

            Builder& visible();
            Builder& not_visible();

            Builder& decorated();
            Builder& not_decorated();

            Builder& focused();
            Builder& not_focused();

            Builder& maximized();
            Builder& not_maximized();

            Builder& swap_interval(uint32_t interval);

            Builder& icon(const std::function<GLFWimage()>& icon_load);

            Builder& cursor(CursorMode cursor);

            Builder& sticky_keys();

            Builder& on_framebuffer_change(FramebufferCallback callback);
            Builder& on_mouse_click(MouseClickCallback callback);
            Builder& on_mouse_move(MouseMoveCallback callback);
            Builder& on_key_press(KeyCallback callback);
            Builder& on_char_press(CharCallback callback);
            Builder& on_scroll(ScrollCallback callback);

            Context build();
        };

        static Builder builder();
    };

    void swap(Context& lhs, Context& rhs) noexcept;
}
