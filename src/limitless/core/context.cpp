#include <limitless/core/context.hpp>
#include <utility>
#include <stb_image.h>

using namespace Limitless;

Context::Context(
    const std::string& title,
    glm::uvec2 size,
    const Context* shared,
    const WindowHints& hints
) : ContextInitializer()
  , ContextState()
  , size {size} {
    // sets window hints for creation
    for (const auto& [hint, value] : hints) {
        glfwWindowHint(static_cast<int>(hint), value);
    }

    window = glfwCreateWindow(size.x, size.y, title.data(), nullptr, shared ? shared->window : nullptr);

    if (!window) {
        throw context_error {"Failed to create window"};
    }

    if (!glew_inited) {
        makeCurrent();
        initializeGLEW();
    }

    init();

    // we created window, reset to default ones
    defaultHints();

    registerContext();

    registerCallbacks();
}

void Context::registerCallbacks() noexcept {
    glfwSetFramebufferSizeCallback(window, Context::framebufferCallback);
    glfwSetMouseButtonCallback(window, Context::mouseclickCallback);
    glfwSetCursorPosCallback(window, Context::mousemoveCallback);
    glfwSetKeyCallback(window, Context::keyboardCallback);
    glfwSetScrollCallback(window, Context::scrollCallback);
    glfwSetCharCallback(window, Context::charCallback);
}

void Context::registerContext() noexcept {
    contexts.emplace(window, this);
}

void Context::unregisterContext() noexcept {
    contexts.erase(window);
}

void Limitless::swap(Context& lhs, Context& rhs) noexcept {
    using std::swap;
    /*
     * we do NOT register nullptr GLFWWindow at Context default constructor (it is private, so GLFWWindow pointer gets nullptr)
     * so have to check that it is present here
     */
    if (lhs && rhs) {
        std::swap(Context::contexts[lhs], Context::contexts[rhs]);
        return;
    }

    if (lhs && !rhs) {
        Context::contexts[lhs] = &rhs;
        return;
    }

    if (!lhs && rhs) {
        Context::contexts[rhs] = &lhs;
        return;
    }

    swap(lhs.window, rhs.window);
    swap(lhs.monitor, rhs.monitor);
    swap(lhs.size, rhs.size);

    swap(lhs.framebuffer_callback, rhs.framebuffer_callback);
    swap(lhs.mouseclick_callback, rhs.mouseclick_callback);
    swap(lhs.mousemove_callback, rhs.mousemove_callback);
    swap(lhs.scroll_callback, rhs.scroll_callback);
    swap(lhs.char_callback, rhs.char_callback);
    swap(lhs.key_callback, rhs.key_callback);
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
        unregisterContext();
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

void Context::pollEvents() const {
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

void Context::framebufferCallback(GLFWwindow* win, int w, int h) {
    contexts[win]->onFramebufferChange({ w, h });
}

void Context::mouseclickCallback(GLFWwindow* win, int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(win, &x, &y);
    contexts[win]->onMouseClick({x, y}, (MouseButton)button, (InputState)action, (Modifier)mods);
}

void Context::mousemoveCallback(GLFWwindow* win, double x, double y) {
    contexts[win]->onMouseMove({ x, y });
}

void Context::keyboardCallback(GLFWwindow *win, int key, int scancode, int action, int modifiers) {
    contexts[win]->onKey(key, scancode, (InputState)action, (Modifier)modifiers);
}

void Context::scrollCallback(GLFWwindow *win, double x, double y) {
    contexts[win]->onScroll({ x, y });
}

void Context::charCallback(GLFWwindow* win, uint32_t utf) {
    contexts[win]->onChar(utf);
}

void Context::onFramebufferChange(glm::uvec2 s) {
    size = s;

    if (framebuffer_callback) {
        framebuffer_callback(size);
    }
}

void Context::onMouseClick(glm::dvec2 pos, MouseButton button, InputState state, Modifier modifier) const {
    if (mouseclick_callback) {
        mouseclick_callback(pos, button, state, modifier);
    }
}

void Context::onMouseMove(glm::dvec2 pos) const {
    if (mousemove_callback) {
        mousemove_callback(pos);
    }
}

void Context::onKey(int key, int scancode, InputState state, Modifier modifier) const {
    if (key_callback) {
        key_callback(key, scancode, state, modifier);
    }
}

void Context::onScroll(glm::dvec2 pos) const {
    if (scroll_callback) {
        scroll_callback(pos);
    }
}

void Context::onChar(uint32_t utf8) const {
    if (char_callback) {
        char_callback(utf8);
    }
}

void Context::setStickyMouseButtons(bool value) const noexcept {
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, value);
}

void Context::setStickyKeys(bool value) const noexcept {
    glfwSetInputMode(window, GLFW_STICKY_KEYS, value);
}

InputState Context::getKey(int key) const noexcept {
    return static_cast<InputState>(glfwGetKey(window, key));
}

bool Context::isPressed(int key) const noexcept {
    return getKey(key) == InputState::Pressed;
}

InputState Context::getMouseButton(MouseButton button) {
    return static_cast<InputState>(glfwGetMouseButton(window, static_cast<uint8_t>(button)));
}

void Context::doOnFramebufferChange(FramebufferCallback callback) {
    framebuffer_callback = std::move(callback);
}

void Context::doOnMouseClick(MouseClickCallback callback) {
    mouseclick_callback = std::move(callback);
}

void Context::doOnMouseMove(MouseMoveCallback callback) {
    mousemove_callback = std::move(callback);
}

void Context::doOnKeyPress(KeyCallback callback) {
    key_callback = std::move(callback);
}

void Context::doOnCharPress(CharCallback callback) {
    char_callback = std::move(callback);
}

void Context::doOnScroll(ScrollCallback callback) {
    scroll_callback = std::move(callback);
}

Context* Context::getCurrentContext() noexcept {
    if (GLFWwindow* window = glfwGetCurrentContext(); window) {
        return contexts.find(window) != contexts.end() ? contexts[window] : nullptr;
    } else {
        return nullptr;
    }
}

void Context::apply(const std::function<void(Context&)>& function) {
    if (Context* ctx = getCurrentContext(); ctx) {
        function(*ctx);
    }
}

Context::Builder Context::builder() {
    return {};
}

Context Context::Builder::build() {
    Context ctx = {ctx_title, ctx_size, ctx_shared, ctx_hints};

    ctx.setSwapInterval(ctx_interval);
    if (ctx_icon) {
        ctx.setWindowIcon(ctx_icon());
    }
    ctx.setCursorMode(ctx_cursor);
    ctx.setStickyKeys(ctx_sticky_keys);

    if (framebuffer_callback)  {
        ctx.doOnFramebufferChange(framebuffer_callback);
    }
    if (mouseclick_callback)  {
        ctx.doOnMouseClick(mouseclick_callback);
    }
    if (mousemove_callback)  {
        ctx.doOnMouseMove(mousemove_callback);
    }
    if (scroll_callback)  {
        ctx.doOnScroll(scroll_callback);
    }
    if (char_callback)  {
        ctx.doOnCharPress(char_callback);
    }
    if (key_callback)  {
        ctx.doOnKeyPress(key_callback);
    }

    //TODO: move in ContextInitializer ?
    // or remove requirements?
    if (!Limitless::ContextInitializer::checkMinimumRequirements()) {
        throw std::runtime_error("Minimum requirements are not met!");
    }

    return ctx;
}

Context::Builder& Context::Builder::title(const std::string &title) {
    ctx_title = title;
    return *this;
}

Context::Builder& Context::Builder::size(glm::uvec2 size) {
    ctx_size = size;
    return *this;
}

Context::Builder& Context::Builder::shared(Context* shared) {
    ctx_shared = shared;
    return *this;
}

Context::Builder& Context::Builder::hints(const WindowHints& hints) {
    ctx_hints = hints;
    return *this;
}

Context::Builder &Context::Builder::resizeable() {
    ctx_hints.emplace_back(WindowHint{WindowHint::Hint::Resizable, GLFW_TRUE});
    return *this;
}

Context::Builder &Context::Builder::not_resizeable() {
    ctx_hints.emplace_back(WindowHint{WindowHint::Hint::Resizable, GLFW_FALSE});
    return *this;
}

Context::Builder &Context::Builder::visible() {
    ctx_hints.emplace_back(WindowHint{WindowHint::Hint::Visible, GLFW_TRUE});
    return *this;
}

Context::Builder &Context::Builder::not_visible() {
    ctx_hints.emplace_back(WindowHint{WindowHint::Hint::Visible, GLFW_FALSE});
    return *this;
}

Context::Builder &Context::Builder::decorated() {
    ctx_hints.emplace_back(WindowHint{WindowHint::Hint::Decorated, GLFW_TRUE});
    return *this;
}

Context::Builder &Context::Builder::not_decorated() {
    ctx_hints.emplace_back(WindowHint{WindowHint::Hint::Decorated, GLFW_FALSE});
    return *this;
}

Context::Builder &Context::Builder::focused() {
    ctx_hints.emplace_back(WindowHint{WindowHint::Hint::Focused, GLFW_TRUE});
    return *this;
}

Context::Builder &Context::Builder::not_focused() {
    ctx_hints.emplace_back(WindowHint{WindowHint::Hint::Focused, GLFW_FALSE});
    return *this;
}

Context::Builder &Context::Builder::maximized() {
    ctx_hints.emplace_back(WindowHint{WindowHint::Hint::Maximized, GLFW_TRUE});
    return *this;
}

Context::Builder &Context::Builder::not_maximized() {
    ctx_hints.emplace_back(WindowHint{WindowHint::Hint::Maximized, GLFW_FALSE});
    return *this;
}

Context::Builder& Context::Builder::swap_interval(uint32_t interval) {
    ctx_interval = interval;
    return *this;
}

Context::Builder& Context::Builder::icon(const std::function<GLFWimage()>& icon_load) {
    ctx_icon = icon_load;
    return *this;
}

Context::Builder &Context::Builder::cursor(CursorMode cursor) {
    ctx_cursor = cursor;
    return *this;
}

Context::Builder &Context::Builder::sticky_keys() {
    ctx_sticky_keys = true;
    return *this;
}

Context::Builder &Context::Builder::on_framebuffer_change(FramebufferCallback callback) {
    framebuffer_callback = std::move(callback);
    return *this;
}

Context::Builder &Context::Builder::on_mouse_click(MouseClickCallback callback) {
    mouseclick_callback = std::move(callback);
    return *this;
}

Context::Builder &Context::Builder::on_mouse_move(MouseMoveCallback callback) {
    mousemove_callback = std::move(callback);
    return *this;
}

Context::Builder &Context::Builder::on_key_press(KeyCallback callback) {
    key_callback = std::move(callback);
    return *this;
}

Context::Builder &Context::Builder::on_char_press(CharCallback callback) {
    char_callback = std::move(callback);
    return *this;
}

Context::Builder &Context::Builder::on_scroll(ScrollCallback callback) {
    scroll_callback = std::move(callback);
    return *this;
}

