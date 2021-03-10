#include <core/context_state.hpp>
#include <core/context_initializer.hpp>
#include <core/context.hpp>

using namespace LimitlessEngine;

bool LimitlessEngine::operator<(const BindingPoint& a, const BindingPoint& b) noexcept
{
    return (a.target == b.target) ? a.point < b.point : a.target < b.target;
}

void ContextState::init() noexcept {
    for (GLint i = 0; i < ContextInitializer::limits.max_texture_units; ++i) {
        texture_bound.emplace(i, 0);
    }
}

void ContextState::registerState(GLFWwindow* window) noexcept {
    std::unique_lock lock{mutex};

    state_map.emplace(window, this);
}

void ContextState::unregisterState(GLFWwindow* window) noexcept {
    std::unique_lock lock{mutex};

    state_map.erase(window);
}

void ContextState::swapStateMap(Context& lhs, Context& rhs) noexcept {
    std::unique_lock lock{mutex};

    // we do not register nullptr window at context default construct, so have to check
    // try to understand that is going on here
    // love implicit casts ;)
    if (lhs && rhs) {
        std::swap(state_map[lhs], state_map[rhs]);
        return;
    }

    if (lhs && !rhs) {
        state_map[lhs] = &rhs;
        return;
    }

    if (!lhs && rhs) {
        state_map[rhs] = &lhs;
        return;
    }
}

void ContextState::clearColor(const glm::vec4& color) noexcept {
    if (clear_color != color) {
        clear_color = color;
        glClearColor(color.x, color.y, color.z, color.w);
    }
}

void ContextState::setBlendFunc(BlendFactor src, BlendFactor dst) noexcept {
    if (src_factor != src || dst_factor != dst) {
        src_factor = src;
        dst_factor = dst;
        glBlendFunc(static_cast<GLenum>(src_factor), static_cast<GLenum>(dst_factor));
    }
}

void ContextState::setBlendColor(const glm::vec4& color) noexcept {
    if (blending_color != color) {
        blending_color = color;
        glBlendColor(color.r, color.g, color.b, color.a);
    }
}

void ContextState::clear(Clear bits) noexcept {
    glClear(static_cast<GLbitfield>(bits));
}

void ContextState::enable(Enable func) noexcept {
    if (!enable_map[func]) {
        glEnable(static_cast<GLenum>(func));
        enable_map[func] = true;
    }
}

void ContextState::disable(Enable func) noexcept {
    if (enable_map[func]) {
        glDisable(static_cast<GLenum>(func));
        enable_map[func] = false;
    }
}

void ContextState::setViewPort(glm::uvec2 viewport_size) noexcept {
    if (viewport != viewport_size) {
        viewport = viewport_size;
        glViewport(0, 0, viewport.x, viewport.y);
    }
}

void ContextState::setDepthFunc(DepthFunc func) noexcept {
    if (depth_func != func) {
        glDepthFunc(static_cast<GLenum>(func));
        depth_func = func;
    }
}

void ContextState::setDepthMask(DepthMask mask) noexcept {
    if (depth_mask != mask) {
        glDepthMask(static_cast<GLenum>(mask));
        depth_mask = mask;
    }
}

void ContextState::setCullFace(CullFace mode) noexcept {
    if (cull_face != mode) {
        glCullFace(static_cast<GLenum>(mode));
        cull_face = mode;
    }
}

void ContextState::setFrontFace(FrontFace mode) noexcept {
    if (front_face != mode) {
        glFrontFace(static_cast<GLenum>(mode));
        front_face = mode;
    }
}

ContextState* ContextState::getState(GLFWwindow* window) noexcept {
    try {
        return state_map.at(window);
    } catch (...) {
        return nullptr;
    }
}

void ContextState::setPolygonMode(CullFace face, PolygonMode mode) noexcept {
    if (face != polygon_face || polygon_mode != mode) {
        glPolygonMode(static_cast<GLenum>(face), static_cast<GLenum>(mode));
        polygon_face = face;
        polygon_mode = mode;
    }
}

bool ContextState::hasState(GLFWwindow* window) noexcept {
    return window ? state_map.find(window) != state_map.end() : false;
}
