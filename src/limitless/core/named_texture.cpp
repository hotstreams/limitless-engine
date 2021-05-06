#include <limitless/core/named_texture.hpp>
#include <limitless/core/context_state.hpp>
#include <limitless/core/context_initializer.hpp>
#include <algorithm>

using namespace Limitless;

NamedTexture::NamedTexture(GLenum _target) noexcept : target{_target} {
    glCreateTextures(target, 1, &id);
}

NamedTexture::~NamedTexture() {
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

void NamedTexture::texStorage2D([[maybe_unused]] GLenum _target, GLsizei levels, GLenum internal_format, glm::uvec2 size) noexcept {
    glTextureStorage2D(id, levels, internal_format, size.x, size.y);
}

void NamedTexture::texStorage3D([[maybe_unused]] GLenum _target, GLsizei levels, GLenum internal_format, glm::uvec3 size) noexcept {
    glTextureStorage3D(id, levels, internal_format, size.x, size.y, size.z);
}

void NamedTexture::texStorage2DMultisample([[maybe_unused]] GLenum _target, uint8_t samples, GLenum internal_format, glm::uvec2 size) noexcept {
    glTextureStorage2DMultisample(id, samples, internal_format, size.x, size.y, GL_FALSE);
}

void NamedTexture::texImage2D(GLenum _target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec2 size, const void* data) noexcept {
    StateTexture::texImage2D(_target, levels, internal_format, format, type, size, data);
}

void NamedTexture::texImage3D(GLenum _target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec3 size, const void *data) noexcept {
    StateTexture::texImage3D(_target, levels, internal_format, format, type, size, data);
}

void NamedTexture::texImage2DMultiSample(GLenum _target, uint8_t samples, GLenum internal_format, glm::uvec3 size) noexcept {
    StateTexture::texImage2DMultiSample(_target, samples, internal_format, size);
}

void NamedTexture::texSubImage2D([[maybe_unused]] GLenum _target, GLint level, GLint xoffset, GLint yoffset, glm::uvec2 size, GLenum format, GLenum type, const void* data) noexcept {
    glTextureSubImage2D(id, level, xoffset, yoffset, size.x, size.y, format, type, data);
}

void NamedTexture::texSubImage3D([[maybe_unused]] GLenum _target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, GLenum format, GLenum type, const void* data) noexcept {
    glTextureSubImage3D(id, level, xoffset, yoffset, zoffset, size.x, size.y, size.z, format, type, data);
}

void NamedTexture::bind([[maybe_unused]] GLenum _target, GLuint index) const {
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (index >= static_cast<GLuint>(ContextInitializer::limits.max_texture_units)) {
            throw std::logic_error{"Failed to bind texture to unit greater than accessible"};
        }

        if (state->texture_bound[index] != id) {
            glBindTextureUnit(index, id);
            state->texture_bound[index] = id;
        }
    }
}

void NamedTexture::generateMipMap([[maybe_unused]] GLenum _target) noexcept {
    glGenerateTextureMipmap(id);
}

void NamedTexture::texParameter([[maybe_unused]] GLenum _target, GLenum name, GLint param) noexcept {
    glTextureParameteri(id, name, param);
}

void NamedTexture::texParameter([[maybe_unused]] GLenum _target, GLenum name, GLfloat param) noexcept {
    glTextureParameterf(id, name, param);
}

void NamedTexture::texParameter([[maybe_unused]] GLenum _target, GLenum name, GLint* params) noexcept {
    glTextureParameteriv(id, name, params);
}

void NamedTexture::texParameter([[maybe_unused]] GLenum _target, GLenum name, GLfloat* params) noexcept {
    glTextureParameterfv(id, name, params);
}

NamedTexture* NamedTexture::clone() const {
    return new NamedTexture(target);
}

void NamedTexture::accept(TextureVisitor& visitor) noexcept {
    visitor.visit(*this);
}

NamedTexture::NamedTexture(NamedTexture&& rhs) noexcept {
    std::swap(id, rhs.id);
    std::swap(target, rhs.target);
}

NamedTexture& NamedTexture::operator=(NamedTexture&& rhs) noexcept {
    std::swap(id, rhs.id);
    std::swap(target, rhs.target);
    return *this;
}
