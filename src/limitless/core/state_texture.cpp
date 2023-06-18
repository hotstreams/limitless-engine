#include <limitless/core/state_texture.hpp>
#include <limitless/core/texture_binder.hpp>
#include <limitless/core/context_initializer.hpp>
#include <limitless/core/context_state.hpp>
#include <algorithm>

using namespace Limitless;

void StateTexture::generateId() noexcept {
    glGenTextures(1, &id);
}

StateTexture::~StateTexture() {
    if (id != 0) {
        if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
            auto& target_map = state->texture_bound;

            std::for_each(target_map.begin(), target_map.end(), [&] (auto& state) {
                auto& [s_unit, s_id] = state;
                if (s_id == id) s_id = 0;
            });

            glDeleteTextures(1, &id);
        }
    }
}

void StateTexture::texStorage2D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec2 size) noexcept {
    bind(target, 0);

    glTexStorage2D(target, levels, internal_format, size.x, size.y);
}

void StateTexture::texStorage3D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec3 size) noexcept {
    bind(target, 0);

    glTexStorage3D(target, levels, internal_format, size.x, size.y, size.z);
}

void StateTexture::texImage2D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec2 size, bool border, const void* data) noexcept {
    bind(target, 0);

    glTexImage2D(target, levels, internal_format, size.x, size.y, border, format, type, data);
}

void StateTexture::texImage3D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec3 size, bool border, const void *data) noexcept {
    bind(target, 0);

    glTexImage3D(target, levels, internal_format, size.x, size.y, size.z, border, format, type, data);
}

void StateTexture::texSubImage2D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, glm::uvec2 size, GLenum format, GLenum type, const void* data) noexcept {
    bind(target, 0);

    glTexSubImage2D(target, levels, xoffset, yoffset, size.x, size.y, format, type, data);
}

void StateTexture::texSubImage3D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, GLenum format, GLenum type, const void* data) noexcept {
    bind(target, 0);

    glTexSubImage3D(target, levels, xoffset, yoffset, zoffset, size.x, size.y, size.z, format, type, data);
}

void StateTexture::generateMipMap(GLenum target) noexcept {
    bind(target, 0);

    glGenerateMipmap(target);
}

void StateTexture::activate(GLuint index) {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (index >= static_cast<GLuint>(ContextInitializer::limits.max_texture_units)) {
            throw std::logic_error{"Failed to activate texture unit greater than accessible"};
        }

        if (state->active_texture != index) {
            glActiveTexture(GL_TEXTURE0 + index);
            state->active_texture = index;
        }
    }
}

void StateTexture::bind(GLenum target, GLuint index) const {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (index >= static_cast<GLuint>(ContextInitializer::limits.max_texture_units)) {
            throw std::logic_error{"Failed to bind texture to unit greater than accessible"};
        }

        if (state->texture_bound[index] != id) {
            activate(index);
            glBindTexture(target, id);
            state->texture_bound[index] = id;
        }
    }
}

StateTexture* StateTexture::clone() const {
    return new StateTexture();
}

void StateTexture::accept(TextureVisitor& visitor) noexcept {
    visitor.visit(*this);
}

StateTexture::StateTexture(StateTexture&& rhs) noexcept {
    std::swap(id, rhs.id);
}

StateTexture& StateTexture::operator=(StateTexture&& rhs) noexcept {
    std::swap(id, rhs.id);
    return *this;
}

StateTexture& StateTexture::setMinFilter(GLenum target, GLenum filter) {
    bind(target, 0);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
    return *this;
}

StateTexture& StateTexture::setMagFilter(GLenum target, GLenum filter) {
    bind(target, 0);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
    return *this;
}

StateTexture& StateTexture::setAnisotropicFilter(GLenum target, float value) {
    bind(target, 0);
    glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY, value);
    return *this;
}

StateTexture& StateTexture::setBorderColor(GLenum target, float* color) {
    bind(target, 0);
    glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, color);
    return *this;
}

StateTexture& StateTexture::setWrapS(GLenum target, GLenum wrap) {
    bind(target, 0);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
    return *this;
}

StateTexture& StateTexture::setWrapT(GLenum target, GLenum wrap) {
    bind(target, 0);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
    return *this;
}

StateTexture& StateTexture::setWrapR(GLenum target, GLenum wrap) {
    bind(target, 0);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);
    return *this;
}

void StateTexture::compressedTexImage2D(GLenum target, GLint level, GLenum internal_format, glm::uvec2 size, bool border, const void *data, std::size_t bytes) noexcept {
    bind(target, 0);

    glCompressedTexImage2D(target, level, internal_format, size.x, size.y, border, bytes, data);
}

void StateTexture::compressedTexImage3D(GLenum target, GLint level, GLenum internal_format, glm::uvec3 size, bool border, const void *data, std::size_t bytes) noexcept {
    bind(target, 0);

    glCompressedTexImage3D(target, level, internal_format, size.x, size.y, size.z, border, bytes, data);
}

void StateTexture::compressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLenum internal_format, glm::uvec2 size, const void *data, std::size_t bytes) noexcept {
    bind(target, 0);

    glCompressedTexSubImage2D(target, level, xoffset, yoffset, size.x, size.y, internal_format, bytes, data);
}

void StateTexture::compressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLenum internal_format, glm::uvec3 size, const void *data, std::size_t bytes) noexcept {
    bind(target, 0);

    glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, size.x, size.y, size.z, internal_format, bytes, data);
}
