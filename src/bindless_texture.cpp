#include <bindless_texture.hpp>

using namespace GraphicsEngine;

BindlessTexture::BindlessTexture(std::shared_ptr<Texture> _texture) : texture(std::move(_texture)) {
    handle = glGetTextureHandleARB(texture->getId());

    makeResident();
}

BindlessTexture::~BindlessTexture() {
    makeNonresident();
}

void BindlessTexture::makeResident() noexcept {
    if (!resident) {
        glMakeTextureHandleResidentARB(handle);
        resident = true;
    }
}

void BindlessTexture::makeNonresident() noexcept {
    if (resident) {
        glMakeTextureHandleNonResidentARB(handle);
        resident = false;
    }
}

void BindlessTexture::bind(GLuint index) const noexcept {
    texture->bind(index);
}

void BindlessTexture::texSubImage2D(GLint xoffset, GLint yoffset, glm::uvec2 size, const void* data) const noexcept{
    texture->texSubImage2D(xoffset, yoffset, size, data);
}

void BindlessTexture::texSubImage3D(GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, const void* data) const noexcept {
    texture->texSubImage3D(xoffset, yoffset, zoffset, size, data);
}

void BindlessTexture::generateMipMap() const noexcept {
    texture->generateMipMap();
}

Texture& BindlessTexture::operator<<(const TexParameter<GLint>& param) noexcept {
    makeNonresident();
    *texture << param;
    makeResident();
    return *this;
}

Texture& BindlessTexture::operator<<(const TexParameter<GLfloat>& param) noexcept {
    *texture << param;
    return *this;
}

Texture& BindlessTexture::operator<<(const TexParameter<GLint*>& param) noexcept {
    *texture << param;
    return *this;
}

Texture& BindlessTexture::operator<<(const TexParameter<GLfloat*>& param) noexcept {
    *texture << param;
    return *this;
}

GLuint BindlessTexture::getId() const noexcept {
    return texture->getId();
}

Texture::Type BindlessTexture::getType() const noexcept {
    return texture->getType();
}

void BindlessTexture::resize(glm::uvec3 size) {
    makeNonresident();
    texture->resize(size);
    makeResident();
}

glm::uvec3 BindlessTexture::getSize() const noexcept {
    return texture->getSize();
}

void BindlessTexture::accept(TextureVisitor &visitor) const noexcept {
    visitor.visit(*this);
}
