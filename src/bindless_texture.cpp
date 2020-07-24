#include <bindless_texture.hpp>
#include <bindless_texture_storage.hpp>

using namespace GraphicsEngine;

BindlessTexture::BindlessTexture(std::shared_ptr<Texture> _texture) : texture(std::move(_texture)) {
    handle = glGetTextureHandleARB(texture->getId());

    makeResident();

    BindlessTextureStorage::init();
    BindlessTextureStorage::add(handle);
}

BindlessTexture::~BindlessTexture() {
    makeNonresident();
    BindlessTextureStorage::remove(handle);
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
    *texture << param;
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
