#include <limitless/core/mutable_texture.hpp>

using namespace LimitlessEngine;

MutableTexture::MutableTexture(std::unique_ptr<ExtensionTexture> _texture, Type target, InternalFormat internal_format, glm::uvec2 size, Format format, DataType data_type, const void* data, const texture_parameters& params) noexcept
    : texture{std::move(_texture)}, internal_format{internal_format}, params{params}, data_type{data_type}, size{size, 0.0f}, format{format}, target{target}  {
    if (params) params(*this);

    texture->texImage2D(static_cast<GLenum>(target), 0, static_cast<GLenum>(internal_format), static_cast<GLenum>(format), static_cast<GLenum>(data_type), size, data);
}

MutableTexture::MutableTexture(std::unique_ptr<ExtensionTexture> _texture, Type target, InternalFormat internal_format, glm::uvec3 size, Format format, DataType data_type, const void* data, const texture_parameters& params) noexcept
    : texture{std::move(_texture)}, internal_format{internal_format}, params{params}, data_type{data_type}, size{size}, format{format}, target{target}  {
    if (params) params(*this);

    switch (target) {
        case Type::Tex2DArray:
            [[fallthrough]];
        case Type::Tex3D:
            texture->texImage3D(static_cast<GLenum>(target), 0, static_cast<GLenum>(internal_format), static_cast<GLenum>(format), static_cast<GLenum>(data_type), size, data);
            break;
        case Type::TexCubeMapArray:
            texture->texImage3D(static_cast<GLenum>(target), 0, static_cast<GLenum>(internal_format), static_cast<GLenum>(format), static_cast<GLenum>(data_type), { size.x, size.y, size.z * 6 }, data);
            break;
        case Type::Tex2D:
            [[fallthrough]];
        case Type::CubeMap:
            break;
    }
}

MutableTexture::MutableTexture(std::unique_ptr<ExtensionTexture> _texture, Type target, InternalFormat internal_format, glm::uvec2 size, Format format, DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params) noexcept
    : texture{std::move(_texture)}, internal_format{internal_format}, params{params}, data_type{data_type}, size{size, 0.0f}, format{format}, target{target}  {
    if (params) params(*this);

    for (uint32_t i = 0; i < data.size(); ++i) {
        texture->texImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, static_cast<GLenum>(internal_format), static_cast<GLenum>(format), static_cast<GLenum>(data_type), size, data[i]);
    }
}

void MutableTexture::texImage2D(GLenum _target, GLsizei _levels, InternalFormat _internal_format, Format _format, DataType _type, glm::uvec2 _size, const void* data) const noexcept {
    texture->texImage2D(_target, _levels, static_cast<GLenum>(_internal_format), static_cast<GLenum>(_format), static_cast<GLenum>(_type), _size, data);
}

void MutableTexture::texImage3D(GLenum _target, GLsizei _levels, InternalFormat _internal_format, Format _format, DataType _type, glm::uvec3 _size, const void *data) const noexcept {
    texture->texImage3D(_target, _levels, static_cast<GLenum>(_internal_format), static_cast<GLenum>(_format), static_cast<GLenum>(_type), _size, data);
}

void MutableTexture::texImage2DMultiSample(uint8_t _samples, InternalFormat _internal_format, glm::uvec3 _size) const noexcept {
    texture->texImage2DMultiSample(static_cast<GLenum>(target), _samples, static_cast<GLenum>(_internal_format), _size);
}

void MutableTexture::texSubImage2D(glm::uvec2 offset, glm::uvec2 _size, const void *data) const noexcept {
    texture->texSubImage2D(static_cast<GLenum>(target), 0, offset.x, offset.y, _size, static_cast<GLenum>(format), static_cast<GLenum>(data_type), data);
}

void MutableTexture::texSubImage3D(glm::uvec3 offset, glm::uvec3 _size, const void *data) const noexcept {
    texture->texSubImage3D(static_cast<GLenum>(target), 0, offset.x, offset.y, offset.z, _size, static_cast<GLenum>(format), static_cast<GLenum>(data_type), data);
}

void MutableTexture::bind(GLuint index) const noexcept {
    texture->bind(static_cast<GLenum>(target), index);
}

void MutableTexture::resize(glm::uvec3 _size) noexcept {
    size = _size;

    if (params) {
        params(*this);
    }

    switch (target) {
        case Type::Tex2D:
            texImage2D(static_cast<GLenum>(target), 0, internal_format, format, data_type, size, nullptr);
            break;
        case Type::Tex2DArray:
        case Type::Tex3D:
            texImage3D(static_cast<GLenum>(target), 0, internal_format, format, data_type, size, nullptr);
            break;
        case Type::CubeMap:
            for (uint32_t i = 0; i < 6; ++i) {
                texImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, format, data_type, size, nullptr);
            }
            break;
        case Type::TexCubeMapArray:
            texImage3D(static_cast<GLenum>(target), 0, internal_format, format, data_type, {size.x, size.y, size.z * 6}, nullptr);
            break;
    }

    if (mipmap) {
        generateMipMap();
    }
}

void MutableTexture::generateMipMap() noexcept {
    mipmap = true;
    texture->generateMipMap(static_cast<GLenum>(target));
}

MutableTexture& MutableTexture::operator<<(const TexParameter<GLint>& param) noexcept {
    texture->texParameter(static_cast<GLenum>(target), param.name, param.param);
    return *this;
}

MutableTexture& MutableTexture::operator<<(const TexParameter<GLfloat>& param) noexcept {
    texture->texParameter(static_cast<GLenum>(target), param.name, param.param);
    return *this;
}

MutableTexture& MutableTexture::operator<<(const TexParameter<GLint*>& param) noexcept {
    texture->texParameter(static_cast<GLenum>(target), param.name, param.param);
    return *this;
}

MutableTexture& MutableTexture::operator<<(const TexParameter<GLfloat*>& param) noexcept {
    texture->texParameter(static_cast<GLenum>(target), param.name, param.param);
    return *this;
}

GLuint MutableTexture::getId() const noexcept {
    return texture->getId();
}

[[nodiscard]] Texture::Type MutableTexture::getType() const noexcept {
    return target;
}

glm::uvec3 MutableTexture::getSize() const noexcept {
    return size;
}

void MutableTexture::accept(TextureVisitor& visitor) noexcept {
    texture->accept(visitor);
}

ExtensionTexture& MutableTexture::getExtensionTexture() noexcept {
    return *texture;
}
