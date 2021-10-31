#include <limitless/core/context_state.hpp>
#include <limitless/core/context_initializer.hpp>
#include <limitless/core/context.hpp>

using namespace Limitless;

bool Limitless::operator<(const BindingPoint& a, const BindingPoint& b) noexcept
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

void ContextState::setLineWidth(float width) noexcept {
    if (line_width != width) {
        glLineWidth(width);
        line_width = width;
    }
}

void ContextState::enable(Capabilities func) noexcept {
    if (!capability_map[func]) {
        glEnable(static_cast<GLenum>(func));
        capability_map[func] = true;
    }
}

void ContextState::setStencilFunc(StencilFunc func, int32_t ref, int32_t mask) noexcept {
    if (stencil_func != func || stencil_func_ref != ref || stencil_func_mask != mask) {
        stencil_func = func;
        stencil_func_ref = ref;
        stencil_func_mask = mask;
        glStencilFunc(static_cast<GLenum>(stencil_func), stencil_func_ref, stencil_func_mask);
    }
}

void ContextState::setStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass) noexcept {
    if (stencil_op[0] != sfail || stencil_op[1] != dpfail || stencil_op[1] != dppass) {
        stencil_op[0] = sfail;
        stencil_op[1] = dpfail;
        stencil_op[2] = dppass;
        glStencilOp(static_cast<GLenum>(sfail), static_cast<GLenum>(dpfail), static_cast<GLenum>(dppass));
    }
}

void ContextState::setStencilMask(int32_t mask) noexcept {
    if (stencil_mask != mask) {
        stencil_mask = mask;
        glStencilMask(mask);
    }
}

void ContextState::disable(Capabilities func) noexcept {
    if (capability_map[func]) {
        glDisable(static_cast<GLenum>(func));
        capability_map[func] = false;
    }
}

void ContextState::setViewPort(glm::uvec2 viewport_size) noexcept {
    if (viewport != viewport_size) {
        viewport = viewport_size;
        glViewport(0, 0, viewport.x, viewport.y);
    }
}

void ContextState::setViewPort(glm::uvec2 position, glm::uvec2 size) noexcept {
//    if (viewport != size) {
//        viewport = size;
        glViewport(position.x, position.y, size.x, size.y);
//    }
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

void ContextState::setScissorTest(glm::uvec2 origin, glm::uvec2 size) noexcept {
    if (scissor_origin != origin || scissor_size != size) {
        glScissor(origin.x, origin.y, size.x, size.y);
        scissor_origin = origin;
        scissor_size = size;
    }
}
