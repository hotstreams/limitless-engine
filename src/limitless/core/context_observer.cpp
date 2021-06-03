#include <limitless/core/context_observer.hpp>

#include <algorithm>

using namespace Limitless;

ContextEventObserver::ContextEventObserver(std::string_view title, glm::uvec2 size, const WindowHints& hints)
    : Context(title, size, hints) {

    callbacks.emplace(window, this);

    glfwSetFramebufferSizeCallback(window, ContextEventObserver::framebufferCallback);
    glfwSetMouseButtonCallback(window, ContextEventObserver::mouseclickCallback);
    glfwSetCursorPosCallback(window, ContextEventObserver::mousemoveCallback);
    glfwSetKeyCallback(window, ContextEventObserver::keyboardCallback);
    glfwSetScrollCallback(window, ContextEventObserver::scrollCallback);
    glfwSetCharCallback(window, ContextEventObserver::charCallback);
}

ContextEventObserver::~ContextEventObserver() {
    callbacks.erase(window);
}

void ContextEventObserver::framebufferCallback(GLFWwindow* win, int w, int h) {
    callbacks[win]->onFramebufferChange({ w, h });
}

void ContextEventObserver::mouseclickCallback(GLFWwindow* win, int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(win, &x, &y);

    const auto pos = glm::dvec2(x, y);

    callbacks[win]->onMouseClick(pos, (MouseButton)button, (InputState)action, (Modifier)mods);
}

void ContextEventObserver::mousemoveCallback(GLFWwindow* win, double x, double y) {
    callbacks[win]->onMouseMove({ x, y });
}

void ContextEventObserver::keyboardCallback(GLFWwindow *win, int key, int scancode, int action, int modifiers) {
    callbacks[win]->onKey(key, scancode, (InputState)action, (Modifier)modifiers);
}

void ContextEventObserver::scrollCallback(GLFWwindow *win, double x, double y) {
    callbacks[win]->onScroll({ x, y });
}

void ContextEventObserver::charCallback(GLFWwindow* win, uint32_t utf) {
    callbacks[win]->onChar(utf);
}

void ContextEventObserver::onFramebufferChange(glm::uvec2 s) {
    size = s;

    setWindowSize(size);

    for (auto* obs : framebuffer_observers) {
        obs->onFramebufferChange(size);
    }
}

void ContextEventObserver::onMouseClick(glm::dvec2 pos, MouseButton button, InputState state, Modifier modifier) const {
    for (auto* ptr : mouseclick_observers) {
        ptr->onMouseClick(pos, button, state, modifier);
    }
}

void ContextEventObserver::onMouseMove(glm::dvec2 pos) const {
    for (auto* ptr : mousemove_observers) {
        ptr->onMouseMove(pos);
    }
}

void ContextEventObserver::onKey(int key, int scancode, InputState state, Modifier modifier) const {
    for (auto* ptr : key_observers)
        ptr->onKey(key, scancode, state, modifier);
}

void ContextEventObserver::onScroll(glm::dvec2 pos) const {
    for (auto* ptr : scroll_observers)
        ptr->onScroll(pos);
}

void ContextEventObserver::onChar(uint32_t utf8) const {
    for (auto* observer : char_observers) {
        observer->onChar(utf8);
    }
}

void ContextEventObserver::registerObserver(FramebufferObserver* obs) {
    framebuffer_observers.emplace_back(obs);
}

void ContextEventObserver::registerObserver(MouseClickObserver* obs) {
    mouseclick_observers.emplace_back(obs);
}

void ContextEventObserver::registerObserver(MouseMoveObserver* obs) {
    mousemove_observers.emplace_back(obs);
}

void ContextEventObserver::registerObserver(KeyObserver* obs) {
    key_observers.emplace_back(obs);
}

void ContextEventObserver::registerObserver(ScrollObserver* obs) {
    scroll_observers.emplace_back(obs);
}

void ContextEventObserver::registerObserver(CharObserver* obs) {
    char_observers.emplace_back(obs);
}

void ContextEventObserver::unregisterObserver(FramebufferObserver* obs) {
    framebuffer_observers.erase(std::find(framebuffer_observers.begin(), framebuffer_observers.end(), obs));
}

void ContextEventObserver::unregisterObserver(MouseClickObserver* obs) {
    mouseclick_observers.erase(std::find(mouseclick_observers.begin(), mouseclick_observers.end(), obs));
}

void ContextEventObserver::unregisterObserver(MouseMoveObserver* obs) {
    mousemove_observers.erase(std::find(mousemove_observers.begin(), mousemove_observers.end(), obs));
}

void ContextEventObserver::unregisterObserver(KeyObserver* obs) {
    key_observers.erase(std::find(key_observers.begin(), key_observers.end(), obs));
}

void ContextEventObserver::unregisterObserver(ScrollObserver* obs) {
    scroll_observers.erase(std::find(scroll_observers.begin(), scroll_observers.end(), obs));
}

void ContextEventObserver::unregisterObserver(CharObserver* obs) {
    char_observers.erase(std::find(char_observers.begin(), char_observers.end(), obs));
}

void ContextEventObserver::setStickyMouseButtons(bool value) const noexcept {
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, value);
}

void ContextEventObserver::setStickyKeys(bool value) const noexcept {
    glfwSetInputMode(window, GLFW_STICKY_KEYS, value);
}

InputState ContextEventObserver::getKey(int key) const noexcept {
    return static_cast<InputState>(glfwGetKey(window, key));
}

bool ContextEventObserver::isPressed(int key) const noexcept {
    return getKey(key) == InputState::Pressed;
}
