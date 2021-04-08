#include <limitless/core/state_texture.hpp>
#include <limitless/core/texture_binder.hpp>
#include <limitless/core/context_initializer.hpp>
#include <limitless/core/context_state.hpp>
#include <algorithm>

using namespace LimitlessEngine;

StateTexture::StateTexture() noexcept {
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

void StateTexture::texStorage2D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec2 size) const noexcept {
    //TextureBinder::bind(target, *this);
    bind(target, 0);

    glTexStorage2D(target, levels, internal_format, size.x, size.y);
}

void StateTexture::texStorage3D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec3 size) const noexcept {
    //TextureBinder::bind(target, *this);
    bind(target, 0);

    glTexStorage3D(target, levels, internal_format, size.x, size.y, size.z);
}

void StateTexture::texStorage2DMultisample(GLenum target, uint8_t samples, GLenum internal_format, glm::uvec2 size) const noexcept {
    //TextureBinder::bind(target, *this);
    bind(target, 0);

    glTexStorage2DMultisample(target, samples, internal_format, size.x, size.y, GL_FALSE);
}

void StateTexture::texImage2D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec2 size, const void* data) const noexcept {
    //TextureBinder::bind(target, *this);
    bind(target, 0);

    glTexImage2D(target, levels, internal_format, size.x, size.y, 0, format, type, data);
}

void StateTexture::texImage3D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec3 size, const void *data) const noexcept {
    //TextureBinder::bind(target, *this);
    bind(target, 0);

    glTexImage3D(target, levels, internal_format, size.x, size.y, size.z, 0, format, type, data);
}

void StateTexture::texImage2DMultiSample(GLenum target, uint8_t samples, GLenum internal_format, glm::uvec3 size) const noexcept {
    //TextureBinder::bind(target, *this);
    bind(target, 0);

    glTexImage2DMultisample(target, samples, internal_format, size.x, size.y, GL_FALSE);
}

void StateTexture::texSubImage2D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, glm::uvec2 size, GLenum format, GLenum type, const void* data) const noexcept {
    //TextureBinder::bind(target, *this);
    bind(target, 0);

    glTexSubImage2D(target, levels, xoffset, yoffset, size.x, size.y, format, type, data);
}

void StateTexture::texSubImage3D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, GLenum format, GLenum type, const void* data) const noexcept {
    //TextureBinder::bind(target, *this);
    bind(target, 0);

    glTexSubImage3D(target, levels, xoffset, yoffset, zoffset, size.x, size.y, size.z, format, type, data);
}

void StateTexture::generateMipMap(GLenum target) const noexcept {
    //TextureBinder::bind(target, *this);
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

void StateTexture::texParameter(GLenum target, GLenum name, GLint param) const noexcept {
    //TextureBinder::bind(target, *this);
    bind(target, 0);

    glTexParameteri(target, name, param);
}

void StateTexture::texParameter(GLenum target, GLenum name, GLfloat param) const noexcept {
    //TextureBinder::bind(target, *this);
    bind(target, 0);

    glTexParameterf(target, name, param);
}

void StateTexture::texParameter(GLenum target, GLenum name, GLint* params) const noexcept {
    //TextureBinder::bind(target, *this);
    bind(target, 0);

    glTexParameteriv(target, name, params);
}

void StateTexture::texParameter(GLenum target, GLenum name, GLfloat* params) const noexcept {
    //TextureBinder::bind(target, *this);
    bind(target, 0);

    glTexParameterfv(target, name, params);
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
