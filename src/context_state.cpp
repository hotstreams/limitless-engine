#include <context_state.hpp>
#include <context_initializer.hpp>
#include <iostream>

using namespace GraphicsEngine;

bool GraphicsEngine::operator<(const BindingPoint& a, const BindingPoint& b) noexcept
{
    return (a.target == b.target) ? a.point < b.point : a.target < b.target;
}

void ContextState::init() noexcept {
    for (GLint i = 0; i < ContextInitializer::limits.max_texture_units; ++i) {
        texture_bound.emplace(i, 0);
    }
}

void ContextState::registerState(GLFWwindow* window) {
    state_map.emplace(window, this);
}

void ContextState::unregisterState(GLFWwindow* window) {
    state_map.erase(window);
}

void ContextState::clearColor(const glm::vec4& color) noexcept {
    if (clear_color != color) {
        clear_color = color;
        glClearColor(color.x, color.y, color.z, color.w);
    }
}

void ContextState::clear(Clear bits) noexcept {
    glClear(static_cast<GLbitfield>(bits));
}

void ContextState::enable(GLenum func) noexcept {
    if (!enable_map[func]) {
        glEnable(func);
        enable_map[func] = true;
    }
}

void ContextState::disable(GLenum func) noexcept {
    if (enable_map[func]) {
        glDisable(func);
        enable_map[func] = false;
    }
}

void ContextState::setViewPort(glm::uvec2 viewport_size) noexcept {
    if (viewport != viewport_size) {
        viewport = viewport_size;
        glViewport(0, 0, viewport.x, viewport.y);
    }
}

void ContextState::setDepthFunc(GLenum func) noexcept {
    if (depth_func != func) {
        glDepthFunc(func);
        depth_func = func;
    }
}

void ContextState::setDepthMask(GLboolean mask) noexcept {
    if (depth_mask != mask) {
        glDepthMask(mask);
        depth_mask = mask;
    }
}

void ContextState::setCullFace(GLenum mode) noexcept {
    if (cull_face != mode) {
        glCullFace(mode);
        cull_face = mode;
    }
}

void ContextState::setFrontFace(GLenum mode) noexcept {
    if (front_face != mode) {
        glFrontFace(mode);
        front_face = mode;
    }
}

ContextState* ContextState::getState(GLFWwindow* window) noexcept {
    try {
        return state_map.at(window);
    } catch (const std::out_of_range& e) {
        return nullptr;
    }
}

void ContextState::setPolygonMode(GLenum mode) noexcept {
    if (polygon_mode != mode) {
        glPolygonMode(GL_FRONT_AND_BACK, mode);
        polygon_mode = mode;
    }
}

bool ContextState::hasState(GLFWwindow* window) noexcept {
    return state_map.find(window) != state_map.end();
}
