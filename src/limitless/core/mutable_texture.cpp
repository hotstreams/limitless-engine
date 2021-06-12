#include <limitless/core/mutable_texture.hpp>
#include <limitless/core/context_initializer.hpp>

using namespace Limitless;

MutableTexture::MutableTexture(std::unique_ptr<ExtensionTexture> _texture,
                               Type _target,
                               InternalFormat _internal_format,
                               glm::uvec2 _size,
                               Format _format,
                               DataType _data_type,
                               bool _border,
                               bool _mipmap,
                               const glm::vec4& _border_color,
                               const void* data,
                               Filter _min,
                               Filter _mag,
                               Wrap _wrap_s,
                               Wrap _wrap_t,
                               Wrap _wrap_r) noexcept
    : texture {std::move(_texture)}
    , border_color {_border_color}
    , internal_format {_internal_format}
    , data_type {_data_type}
    , size {_size, 0.0f}
    , format {_format}
    , target {_target}
    , min {_min}
    , mag {_mag}
    , wrap_r {_wrap_r}
    , wrap_s {_wrap_s}
    , wrap_t {_wrap_t}
    , mipmap {_mipmap}
    , border {_border} {

    setParameters();

    texture->texImage2D(static_cast<GLenum>(target), 0, static_cast<GLenum>(internal_format), static_cast<GLenum>(format), static_cast<GLenum>(data_type), size, border, data);

    if (mipmap) {
        generateMipMap();
    }
}

MutableTexture::MutableTexture(std::unique_ptr<ExtensionTexture> _texture,
                               Type _target,
                               InternalFormat _internal_format,
                               glm::uvec3 _size,
                               Format _format,
                               DataType _data_type,
                               bool _border,
                               bool _mipmap,
                               const glm::vec4& _border_color,
                               const void* data,
                               Filter _min,
                               Filter _mag,
                               Wrap _wrap_s,
                               Wrap _wrap_t,
                               Wrap _wrap_r) noexcept
    : texture {std::move(_texture)}
    , border_color {_border_color}
    , internal_format {_internal_format}
    , data_type {_data_type}
    , size {_size}
    , format {_format}
    , target {_target}
    , min {_min}
    , mag {_mag}
    , wrap_r {_wrap_r}
    , wrap_s {_wrap_s}
    , wrap_t {_wrap_t}
    , mipmap {_mipmap}
    , border {_border}  {

    setParameters();

    switch (target) {
        case Type::Tex2DArray:
            [[fallthrough]];
        case Type::Tex3D:
            texture->texImage3D(static_cast<GLenum>(target), 0, static_cast<GLenum>(internal_format), static_cast<GLenum>(format), static_cast<GLenum>(data_type), size, border, data);
            break;
        case Type::TexCubeMapArray:
            texture->texImage3D(static_cast<GLenum>(target), 0, static_cast<GLenum>(internal_format), static_cast<GLenum>(format), static_cast<GLenum>(data_type), { size.x, size.y, size.z * 6 }, border, data);
            break;
        case Type::Tex2D:
            [[fallthrough]];
        case Type::CubeMap:
            break;
    }

    if (mipmap) {
        generateMipMap();
    }
}

MutableTexture::MutableTexture(std::unique_ptr<ExtensionTexture> _texture,
                               Type _target,
                               InternalFormat _internal_format,
                               glm::uvec2 _size,
                               Format _format,
                               DataType _data_type,
                               bool _border,
                               bool _mipmap,
                               const glm::vec4& _border_color,
                               const std::array<void*, 6>& data,
                               Filter _min,
                               Filter _mag,
                               Wrap _wrap_s,
                               Wrap _wrap_t,
                               Wrap _wrap_r) noexcept
    : texture {std::move(_texture)}
    , border_color {_border_color}
    , internal_format {_internal_format}
    , data_type {_data_type}
    , size {_size, 0.0f}
    , format {_format}
    , target {_target}
    , min {_min}
    , mag {_mag}
    , wrap_r {_wrap_r}
    , wrap_s {_wrap_s}
    , wrap_t {_wrap_t}
    , mipmap {_mipmap}
    , border {_border}  {

    setParameters();

    for (size_t i = 0; i < data.size(); ++i) {
        texture->texImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, static_cast<GLenum>(internal_format), static_cast<GLenum>(format), static_cast<GLenum>(data_type), size, border, data[i]);
    }

    if (mipmap) {
        generateMipMap();
    }
}

void MutableTexture::texImage2D(GLenum _target, GLsizei _levels, InternalFormat _internal_format, Format _format, DataType _type, glm::uvec2 _size, const void* data) const noexcept {
    texture->texImage2D(_target, _levels, static_cast<GLenum>(_internal_format), static_cast<GLenum>(_format), static_cast<GLenum>(_type), _size, border, data);
}

void MutableTexture::texImage3D(GLenum _target, GLsizei _levels, InternalFormat _internal_format, Format _format, DataType _type, glm::uvec3 _size, const void *data) const noexcept {
    texture->texImage3D(_target, _levels, static_cast<GLenum>(_internal_format), static_cast<GLenum>(_format), static_cast<GLenum>(_type), _size, border, data);
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

    setParameters();

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

MutableTexture& MutableTexture::setMinFilter(Texture::Filter filter) {
    min = filter;
    texture->setMinFilter(static_cast<GLenum>(target), static_cast<GLenum>(filter));
    return *this;
}

MutableTexture& MutableTexture::setMagFilter(Texture::Filter filter) {
    mag = filter;
    texture->setMagFilter(static_cast<GLenum>(target), static_cast<GLenum>(filter));
    return *this;
}

MutableTexture& MutableTexture::setAnisotropicFilter(float value) {
    anisotropic = value;
    texture->setAnisotropicFilter(static_cast<GLenum>(target), value);
    return *this;
}

MutableTexture& MutableTexture::setAnisotropicFilterMax() {
    anisotropic = ContextInitializer::limits.anisotropic_max;
    setAnisotropicFilter(anisotropic);
    return *this;
}

MutableTexture& MutableTexture::setBorderColor(const glm::vec4& color) {
    border_color = color;
    texture->setBorderColor(static_cast<GLenum>(target), &border_color[0]);
    return *this;
}

MutableTexture& MutableTexture::setWrapS(Wrap wrap) {
    wrap_s = wrap;
    texture->setWrapS(static_cast<GLenum>(target), static_cast<GLenum>(wrap_s));
    return *this;
}

MutableTexture& MutableTexture::setWrapT(Wrap wrap) {
    wrap_t = wrap;
    texture->setWrapT(static_cast<GLenum>(target), static_cast<GLenum>(wrap_t));
    return *this;
}

MutableTexture& MutableTexture::setWrapR(Wrap wrap) {
    wrap_r = wrap;
    texture->setWrapR(static_cast<GLenum>(target), static_cast<GLenum>(wrap_r));
    return *this;
}

void MutableTexture::setParameters() {
    setMinFilter(min);
    setMagFilter(mag);
    setWrapS(wrap_s);
    setWrapT(wrap_t);
    setWrapR(wrap_r);
}
