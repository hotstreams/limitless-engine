#include <limitless/core/immutable_texture.hpp>

using namespace LimitlessEngine;

ImmutableTexture::ImmutableTexture(std::unique_ptr<ExtensionTexture> _texture, Type target, GLsizei levels, InternalFormat internal_format, glm::uvec2 size, Format format, DataType data_type, const void* data, const texture_parameters& params) noexcept
    : texture{std::move(_texture)}
    , internal_format{internal_format}
    , params{params}
    , data_type{data_type}
    , levels{levels}
    , size{size, 0.0f}
    , format{format}
    , target{target} {

    if (params) {
        params(*this);
    }

    texStorage2D(static_cast<GLenum>(target), levels, internal_format, size);

    if (data) {
        ImmutableTexture::texSubImage2D({0, 0}, size, data);
    }
}

ImmutableTexture::ImmutableTexture(std::unique_ptr<ExtensionTexture> _texture, Type target, GLsizei levels, InternalFormat internal_format, glm::uvec3 size, Format format, DataType data_type, const void* data, const texture_parameters& params) noexcept
        : texture{std::move(_texture)}, internal_format{internal_format}, params{params}, data_type{data_type}, levels{levels}, size{size}, format{format}, target{target} {
    if (params) params(*this);

    if (target == Type::Tex2DArray || target == Type::Tex3D) {
        texStorage3D(static_cast<GLenum>(target), levels, internal_format, size);
        if (data) {
            ImmutableTexture::texSubImage3D({0, 0, 0}, size, data);
        }
    }

    if (target == Type::TexCubeMapArray) {
        texStorage3D(static_cast<GLenum>(target), levels, internal_format, { size.x, size.y, size.z * 6 });
        if (data) {
            ImmutableTexture::texSubImage3D({0, 0, 0}, { size.x, size.y, size.z * 6 }, data);
        }
    }
}

ImmutableTexture::ImmutableTexture(std::unique_ptr<ExtensionTexture> _texture, Type target, GLsizei levels, InternalFormat internal_format, glm::uvec2 size, Format format, DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params) noexcept
        : texture{std::move(_texture)}, internal_format{internal_format}, params{params}, data_type{data_type}, levels{levels}, size{size, 0.0f}, format{format}, target{target} {
    if (params) params(*this);

    texStorage2D(GL_TEXTURE_CUBE_MAP, levels, internal_format, size);

    for (auto* ptr : data) {
        if (ptr) {
//            ImmutableTexture::texSubImage2D({0, 0}, size, ptr);
//            texture->texSubImage3D(static_cast<GLenum>(target), 0, offset.x, offset.y, offset.z, _size, static_cast<GLenum>(format), static_cast<GLenum>(data_type), data);
        }
    }
}

void ImmutableTexture::texStorage2D(GLenum _target, GLsizei _levels, InternalFormat _internal_format, glm::uvec2 _size) const noexcept {
    texture->texStorage2D(_target, _levels, static_cast<GLenum>(_internal_format), _size);
}

void ImmutableTexture::texStorage3D(GLenum _target, GLsizei _levels, InternalFormat _internal_format, glm::uvec3 _size) const noexcept {
    texture->texStorage3D(_target, _levels, static_cast<GLenum>(_internal_format), _size);
}

void ImmutableTexture::texStorage2DMultiSample(GLenum _target, uint8_t _samples, InternalFormat _internal_format, glm::uvec2 _size) const noexcept {
    texture->texStorage2DMultisample(_target, _samples, static_cast<GLenum>(_internal_format), _size);
}

void ImmutableTexture::texSubImage2D(glm::uvec2 offset, glm::uvec2 _size, const void* data) const noexcept {
    texture->texSubImage2D(static_cast<GLenum>(target), 0, offset.x, offset.y, _size, static_cast<GLenum>(format), static_cast<GLenum>(data_type), data);
}

void ImmutableTexture::texSubImage3D(glm::uvec3 offset, glm::uvec3 _size, const void *data) const noexcept {
    texture->texSubImage3D(static_cast<GLenum>(target), 0, offset.x, offset.y, offset.z, _size, static_cast<GLenum>(format), static_cast<GLenum>(data_type), data);
}

void ImmutableTexture::generateMipMap() noexcept {
    mipmap = true;
    texture->generateMipMap(static_cast<GLenum>(target));
}

void ImmutableTexture::bind(GLuint index) const noexcept {
    texture->bind(static_cast<GLenum>(target), index);
}

ImmutableTexture& ImmutableTexture::operator<<(const TexParameter<GLint>& param) noexcept {
    texture->texParameter(static_cast<GLenum>(target), param.name, param.param);
    return *this;
}

ImmutableTexture& ImmutableTexture::operator<<(const TexParameter<GLfloat>& param) noexcept {
    texture->texParameter(static_cast<GLenum>(target), param.name, param.param);
    return *this;
}

ImmutableTexture& ImmutableTexture::operator<<(const TexParameter<GLint*>& param) noexcept {
    texture->texParameter(static_cast<GLenum>(target), param.name, param.param);
    return *this;
}

ImmutableTexture& ImmutableTexture::operator<<(const TexParameter<GLfloat*>& param) noexcept {
    texture->texParameter(static_cast<GLenum>(target), param.name, param.param);
    return *this;
}

void ImmutableTexture::accept(TextureVisitor& visitor) noexcept {
    texture->accept(visitor);
}

void ImmutableTexture::resize(glm::uvec3 _size) noexcept {
    size = _size;

    switch (target) {
        case Type::Tex2D: {
            ImmutableTexture new_texture{std::unique_ptr<ExtensionTexture>(texture->clone()), target, levels, internal_format, glm::uvec2{size}, format, data_type, nullptr, params};
            *this = std::move(new_texture);
            break;
        }
        case Type::Tex2DArray:
        case Type::TexCubeMapArray:
        case Type::Tex3D: {
            ImmutableTexture new_texture{std::unique_ptr<ExtensionTexture>(texture->clone()), target, levels, internal_format, size, format, data_type, nullptr, params};
            *this = std::move(new_texture);
            break;
        }
        case Type::CubeMap: {
            ImmutableTexture new_texture{std::unique_ptr<ExtensionTexture>(texture->clone()), target, levels, internal_format, glm::uvec2{size}, format, data_type, std::array<void *, 6>{}, params};
            *this = std::move(new_texture);
            break;
        }
    }

    if (mipmap) {
        generateMipMap();
    }
}

GLuint ImmutableTexture::getId() const noexcept {
    return texture->getId();
}

Texture::Type ImmutableTexture::getType() const noexcept {
    return target;
}

glm::uvec3 ImmutableTexture::getSize() const noexcept {
    return size;
}

ExtensionTexture& ImmutableTexture::getExtensionTexture() noexcept {
    return *texture;
}
