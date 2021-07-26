#include <limitless/core/immutable_texture.hpp>
#include <limitless/core/context_initializer.hpp>

using namespace Limitless;

ImmutableTexture::ImmutableTexture(std::unique_ptr<ExtensionTexture> _texture,
                                   Type _target,
                                   GLsizei _levels,
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
    , levels {_levels}
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

    texStorage2D(static_cast<GLenum>(target), levels, internal_format, size);

    if (data) {
        ImmutableTexture::texSubImage2D({0, 0}, size, data);
    }

    if (mipmap) {
        generateMipMap();
    }
}

ImmutableTexture::ImmutableTexture(std::unique_ptr<ExtensionTexture> _texture,
                                   Type _target,
                                   GLsizei _levels,
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
    , levels {_levels}
    , size {_size}
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

    if (mipmap) {
        generateMipMap();
    }
}

ImmutableTexture::ImmutableTexture(std::unique_ptr<ExtensionTexture> _texture,
                                   Type _target,
                                   GLsizei _levels,
                                   InternalFormat _internal_format,
                                   glm::uvec2 _size,
                                   Format _format,
                                   DataType _data_type,
                                   bool _border,
                                   bool _mipmap,
                                   const glm::vec4& _border_color,
                                   [[maybe_unused]] const std::array<void*, 6>& data,
                                   Filter _min,
                                   Filter _mag,
                                   Wrap _wrap_s,
                                   Wrap _wrap_t,
                                   Wrap _wrap_r) noexcept
    : texture {std::move(_texture)}
    , border_color {_border_color}
    , internal_format {_internal_format}
    , data_type {_data_type}
    , levels {_levels}
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

    texStorage2D(GL_TEXTURE_CUBE_MAP, levels, internal_format, size);

    //TODO: test cubemaparray constructor
//    for (auto* ptr : data) {
//        if (ptr) {
////            ImmutableTexture::texSubImage2D({0, 0}, size, ptr);
////            texture->texSubImage3D(static_cast<GLenum>(target), 0, offset.x, offset.y, offset.z, _size, static_cast<GLenum>(format), static_cast<GLenum>(data_type), data);
//        }
//    }

    if (mipmap) {
        generateMipMap();
    }
}

void ImmutableTexture::texStorage2D(GLenum _target, GLsizei _levels, InternalFormat _internal_format, glm::uvec2 _size) const noexcept {
    texture->texStorage2D(_target, _levels, static_cast<GLenum>(_internal_format), _size);
}

void ImmutableTexture::texStorage3D(GLenum _target, GLsizei _levels, InternalFormat _internal_format, glm::uvec3 _size) const noexcept {
    texture->texStorage3D(_target, _levels, static_cast<GLenum>(_internal_format), _size);
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

void ImmutableTexture::accept(TextureVisitor& visitor) noexcept {
    texture->accept(visitor);
}

void ImmutableTexture::resize(glm::uvec3 _size) noexcept {
    size = _size;

    switch (target) {
        case Type::Tex2D: {
            ImmutableTexture new_texture{std::unique_ptr<ExtensionTexture>(texture->clone()), target, levels, internal_format, glm::uvec2{size}, format, data_type, border, mipmap, border_color, nullptr, min, mag, wrap_s, wrap_t, wrap_r};
            *this = std::move(new_texture);
            break;
        }
        case Type::Tex2DArray:
        case Type::TexCubeMapArray:
        case Type::Tex3D: {
            ImmutableTexture new_texture{std::unique_ptr<ExtensionTexture>(texture->clone()), target, levels, internal_format, size, format, data_type, border, mipmap, border_color, nullptr, min, mag, wrap_s, wrap_t, wrap_r};
            *this = std::move(new_texture);
            break;
        }
        case Type::CubeMap: {
            ImmutableTexture new_texture{std::unique_ptr<ExtensionTexture>(texture->clone()), target, levels, internal_format, glm::uvec2{size}, format, data_type, border, mipmap, border_color, std::array<void *, 6>{}, min, mag, wrap_s, wrap_t, wrap_r};
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

ImmutableTexture& ImmutableTexture::setMinFilter(Texture::Filter filter) {
    min = filter;
    texture->setMinFilter(static_cast<GLenum>(target), static_cast<GLenum>(filter));
    return *this;
}

ImmutableTexture& ImmutableTexture::setMagFilter(Texture::Filter filter) {
    mag = filter;
    texture->setMagFilter(static_cast<GLenum>(target), static_cast<GLenum>(filter));
    return *this;
}

ImmutableTexture& ImmutableTexture::setAnisotropicFilter(float value) {
    anisotropic = value;
    texture->setAnisotropicFilter(static_cast<GLenum>(target), value);
    return *this;
}

ImmutableTexture& ImmutableTexture::setAnisotropicFilterMax() {
    anisotropic = ContextInitializer::limits.anisotropic_max;
    setAnisotropicFilter(anisotropic);
    return *this;
}

ImmutableTexture& ImmutableTexture::setBorderColor(const glm::vec4& color) {
    border_color = color;
    texture->setBorderColor(static_cast<GLenum>(target), &border_color[0]);
    return *this;
}

ImmutableTexture& ImmutableTexture::setWrapS(Wrap wrap) {
    wrap_s = wrap;
    texture->setWrapS(static_cast<GLenum>(target), static_cast<GLenum>(wrap_s));
    return *this;
}

ImmutableTexture& ImmutableTexture::setWrapT(Wrap wrap) {
    wrap_t = wrap;
    texture->setWrapT(static_cast<GLenum>(target), static_cast<GLenum>(wrap_t));
    return *this;
}

ImmutableTexture& ImmutableTexture::setWrapR(Wrap wrap) {
    wrap_r = wrap;
    texture->setWrapR(static_cast<GLenum>(target), static_cast<GLenum>(wrap_r));
    return *this;
}

void ImmutableTexture::setParameters() {
    setMinFilter(min);
    setMagFilter(mag);
    setWrapS(wrap_s);
    setWrapT(wrap_t);
    setWrapR(wrap_r);

    if (border) {
        setBorderColor(border_color);
    }
}
