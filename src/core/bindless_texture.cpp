#include <core/bindless_texture.hpp>
#include <core/context_state.hpp>

using namespace GraphicsEngine;

BindlessTexture::BindlessTexture(ExtensionTexture* texture) : texture(texture) {
}

void BindlessTexture::makeBindless() const noexcept {
    if (!handle) {
        handle = glGetTextureHandleARB(texture->getId());
    }
}

BindlessTexture::~BindlessTexture() {
    makeNonresident();
    //TODO: del handle from ctx?
}

void BindlessTexture::makeResident() const noexcept {
    makeBindless();
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (!state->texture_resident[handle]) {
            glMakeTextureHandleResidentARB(glGetTextureHandleARB(texture->getId()));
            state->texture_resident[handle] = true;
        }
    }
}

void BindlessTexture::makeNonresident() const noexcept {
    if (!handle) return;
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (state->texture_resident[handle]) {
            glMakeTextureHandleNonResidentARB(handle);
            state->texture_resident[handle] = false;
        }
    }
}

BindlessTexture *BindlessTexture::clone() const {
    return new BindlessTexture(texture->clone());
}

void BindlessTexture::texStorage2D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec2 size) const noexcept {
    makeNonresident();
    texture->texStorage2D(target, levels, internal_format, size);
}

void BindlessTexture::texStorage3D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec3 size) const noexcept {
    makeNonresident();
    texture->texStorage3D(target, levels, internal_format, size);
}

void BindlessTexture::texStorage2DMultisample(GLenum target, uint8_t samples, GLenum internal_format, glm::uvec2 size) const noexcept {
    makeNonresident();
    texture->texStorage2DMultisample(target, samples, internal_format, size);
}

void BindlessTexture::texImage2D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec2 size, const void *data) const noexcept {
    makeNonresident();
    texture->texImage2D(target, levels, internal_format, format, type, size, data);
}

void BindlessTexture::texImage3D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec3 size, const void *data) const noexcept {
    makeNonresident();
    texture->texImage3D(target, levels, internal_format, format, type, size, data);
}

void BindlessTexture::texImage2DMultiSample(GLenum target, uint8_t samples, GLenum internal_format, glm::uvec3 size) const noexcept {
    makeNonresident();
    texture->texImage2DMultiSample(target, samples, internal_format, size);
}

void BindlessTexture::texSubImage2D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, glm::uvec2 size, GLenum format, GLenum type, const void *data) const noexcept {
    makeNonresident();
    texture->texSubImage2D(target, levels, xoffset, yoffset, size, format, type, data);
}

void BindlessTexture::texSubImage3D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, GLenum format, GLenum type, const void *data) const noexcept {
    makeNonresident();
    texture->texSubImage3D(target, levels, xoffset, yoffset, zoffset, size, format, type, data);
}

void BindlessTexture::generateMipMap(GLenum target) const noexcept {
    makeNonresident();
    texture->generateMipMap(target);
}

void BindlessTexture::bind(GLenum target, GLuint index) const noexcept {
    texture->bind(target, index);
}

void BindlessTexture::texParameter(GLenum target, GLenum name, GLint param) const noexcept {
    makeNonresident();
    texture->texParameter(target, name, param);
}

void BindlessTexture::texParameter(GLenum target, GLenum name, GLfloat param) const noexcept {
    makeNonresident();
    texture->texParameter(target, name, param);
}

void BindlessTexture::texParameter(GLenum target, GLenum name, GLint *params) const noexcept {
    makeNonresident();
    texture->texParameter(target, name, params);
}

void BindlessTexture::texParameter(GLenum target, GLenum name, GLfloat *params) const noexcept {
    makeNonresident();
    texture->texParameter(target, name, params);
}

void BindlessTexture::accept(TextureVisitor& visitor) noexcept {
    visitor.visit(*this);
}

BindlessTexture::BindlessTexture(BindlessTexture&& rhs) noexcept {
    std::swap(handle, rhs.handle);
    std::swap(texture, rhs.texture);
}

BindlessTexture& BindlessTexture::operator=(BindlessTexture&& rhs) noexcept {
    std::swap(handle, rhs.handle);
    std::swap(texture, rhs.texture);
    return *this;
}
