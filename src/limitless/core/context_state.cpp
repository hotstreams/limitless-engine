#include <limitless/core/context_state.hpp>
#include <limitless/core/context_initializer.hpp>
#include <limitless/core/context.hpp>

using namespace Limitless;

void ContextState::init() noexcept {
    for (GLint i = 0; i < ContextInitializer::limits.max_texture_units; ++i) {
        texture_bound.emplace(i, 0);
    }

    for (GLuint i = 0; i < (GLuint)ContextInitializer::limits.shader_storage_max_count; ++i) {
        buffer_point.emplace(BindingPoint{Buffer::Type::ShaderStorage, i}, 0);
    }

    for (GLuint i = 0; i < (GLuint)ContextInitializer::limits.uniform_buffer_max_count; ++i) {
        buffer_point.emplace(BindingPoint{Buffer::Type::Uniform, i}, 0);
    }

    capability_map.emplace(Capabilities::DepthTest, false);
    capability_map.emplace(Capabilities::Blending, false);
    capability_map.emplace(Capabilities::ProgramPointSize, false);
    capability_map.emplace(Capabilities::ScissorTest, false);
    capability_map.emplace(Capabilities::StencilTest, false);
    capability_map.emplace(Capabilities::CullFace, false);

    buffer_target.emplace(Buffer::Type::Element, 0);
    buffer_target.emplace(Buffer::Type::Array, 0);
    buffer_target.emplace(Buffer::Type::ShaderStorage, 0);
    buffer_target.emplace(Buffer::Type::Uniform, 0);
}

void ContextState::registerState(GLFWwindow* window) noexcept {
    std::unique_lock lock{mutex};

    state_map.emplace(window, this);
}

void ContextState::unregisterState(GLFWwindow* window) noexcept {
    std::unique_lock lock{mutex};

    state_map.erase(window);
}

void ContextState::swap_state_map(Context& lhs, Context& rhs) noexcept {
    std::unique_lock lock{mutex};

    /*
     * we do NOT register nullptr GLFWWindow at Context default constructor (it is private, so GLFWWindow pointer gets nullptr)
     * so have to check that it is present here
     */

    // love implicit casts btw ;)

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
    if (blend_color != color) {
        blend_color = color;
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

void ContextState::setPixelStore(PixelStore name, GLint param) noexcept {
    if (pixel_pack != name || pixel_param != param) {
        glPixelStorei(static_cast<GLenum>(name), param);
        pixel_pack = name;
        pixel_param = param;
    }
}

void ContextState::disable(Capabilities func) noexcept {
    if (capability_map[func]) {
        glDisable(static_cast<GLenum>(func));
        capability_map[func] = false;
    }
}

void ContextState::setViewPort(glm::uvec2 viewport_size) noexcept {
    if (glm::uvec2{viewport.z, viewport.w} != viewport_size) {
        viewport.z = viewport_size.x;
        viewport.w = viewport_size.y;
        glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
    }
}

void ContextState::setViewPort(glm::uvec4 new_viewport) noexcept {
    if (viewport != new_viewport) {
        viewport = new_viewport;
        glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
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

void ContextState::setScissorTest(glm::uvec2 origin, glm::uvec2 size) noexcept {
//    if (scissor_viewport != origin || scissor_size != size) {
        glScissor(origin.x, origin.y, size.x, size.y);
//        scissor_origin = origin;
//        scissor_size = size;
//    }
}
