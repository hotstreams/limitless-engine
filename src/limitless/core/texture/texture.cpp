#include <limitless/core/texture/texture.hpp>

#include <limitless/core/texture/extension_texture.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/core/context_initializer.hpp>
#include <limitless/core/framebuffer.hpp>
#include <utility>

using namespace Limitless;

void Texture::storage(const void* data) {
    setParameters();

    if (is2D()) {
        texture->texStorage2D(static_cast<GLenum>(target), levels, static_cast<GLenum>(internal_format), static_cast<glm::uvec2>(size));
        if (data) {
            subImage(0, glm::uvec2{0, 0}, static_cast<glm::uvec2>(size), data);
        }
    }

    if (is3D()) {
        texture->texStorage3D(static_cast<GLenum>(target), levels, static_cast<GLenum>(internal_format), size);
        if (data) {
            subImage(0, {0, 0, 0}, size, data);
        }
    }

    if (isCubemapArray()) {
        texture->texStorage3D(static_cast<GLenum>(target), levels, static_cast<GLenum>(internal_format), { size.x, size.y, size.z * 6 });
        if (data) {
            subImage(0, {0, 0, 0}, { size.x, size.y, size.z * 6 }, data);
        }
    }

    if (mipmap) {
        generateMipMap();
    }
}

void Texture::storage([[maybe_unused]] const std::array<void*, 6>& data) {
    setParameters();

    throw std::runtime_error("Not yet implemented");
}

void Texture::image(const void* data) {
    setParameters();

    if (is2D()) {
        image(0, static_cast<glm::uvec2>(size), data);
    }

    if (is3D()) {
        image(0, size, data);
    }

    if (isCubemapArray()) {
        image(0, { size.x, size.y, size.z * 6 }, data);
    }

    if (mipmap) {
        generateMipMap();
    }
}

void Texture::image(const std::array<void*, 6>& data) {
    setParameters();

    for (std::size_t i = 0; i < data.size(); ++i) {
        texture->texImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, static_cast<GLenum>(internal_format), static_cast<GLenum>(format), static_cast<GLenum>(data_type), size, false, data[i]);
    }

    if (mipmap) {
        generateMipMap();
    }
}

void Texture::image(const std::vector<const void*>& layers) {
    setParameters();

    texture->texImage3D(static_cast<GLenum>(target), 0, static_cast<GLenum>(internal_format), static_cast<GLenum>(format), static_cast<GLenum>(data_type), {size.x, size.y, layers.size()}, false,nullptr);

    for (uint32_t i = 0; i < layers.size(); ++i) {
        texture->texSubImage3D(static_cast<GLenum>(target), 0, 0, 0, i, {size.x, size.y, 1}, static_cast<GLenum>(format), static_cast<GLenum>(data_type), layers.at(i));
    }

    if (mipmap) {
        generateMipMap();
    }
}

void Texture::image(uint32_t level, glm::uvec2 _size, const void* data) {
    setParameters();
    texture->texImage2D(static_cast<GLenum>(target), level, static_cast<GLenum>(internal_format), static_cast<GLenum>(format), static_cast<GLenum>(data_type), _size, false, data);
}

void Texture::image(uint32_t level, glm::uvec3 _size, const void* data) {
    setParameters();
    texture->texImage3D(static_cast<GLenum>(target), level, static_cast<GLenum>(internal_format), static_cast<GLenum>(format), static_cast<GLenum>(data_type), _size, false, data);
}

void Texture::compressedImage(const void* data, std::size_t byte_count) {
    setParameters();
    if (is2D()) {
        compressedImage(0, static_cast<glm::uvec2>(size), data, byte_count);
    }
}

void Texture::compressedImage(uint32_t level, glm::uvec2 _size, const void* data, std::size_t byte_count) {
    setParameters();
    texture->compressedTexImage2D(static_cast<GLenum>(target), level, static_cast<GLenum>(internal_format), _size, false, data, byte_count);
}

void Texture::compressedImage(uint32_t level, glm::uvec3 _size, const void* data, std::size_t byte_count) {
    setParameters();
    texture->compressedTexImage3D(static_cast<GLenum>(target), level, static_cast<GLenum>(internal_format), _size, false, data, byte_count);
}

void Texture::subImage(uint32_t level, glm::uvec2 offset, glm::uvec2 _size, const void* data) {
    setParameters();
    texture->texSubImage2D(static_cast<GLenum>(target), level, offset.x, offset.y, _size, static_cast<GLenum>(format), static_cast<GLenum>(data_type), data);
}

void Texture::subImage(uint32_t level, glm::uvec3 offset, glm::uvec3 _size, const void* data) {
    setParameters();
    texture->texSubImage3D(static_cast<GLenum>(target), level, offset.x, offset.y, offset.z, _size, static_cast<GLenum>(format), static_cast<GLenum>(data_type), data);
}

void Texture::compressedSubImage(uint32_t level, glm::uvec2 offset, glm::uvec2 _size, const void* data, std::size_t byte_count) {
    setParameters();
    texture->compressedTexSubImage2D(static_cast<GLenum>(target), level, offset.x, offset.y, static_cast<GLenum>(internal_format), _size, data, byte_count);
}

void Texture::compressedSubImage(uint32_t level, glm::uvec3 offset, glm::uvec3 _size, const void* data, std::size_t byte_count) {
    setParameters();
    texture->compressedTexSubImage3D(static_cast<GLenum>(target), level, offset.x, offset.y, offset.z, static_cast<GLenum>(internal_format), _size, data, byte_count);
}

void Texture::generateMipMap() {
    texture->generateMipMap(static_cast<GLenum>(target));
}

void Texture::bind(GLuint index) const {
    texture->bind(static_cast<GLenum>(target), index);
}

std::shared_ptr<Texture> Texture::clone() {
    return clone(size);
}

std::shared_ptr<Texture> Texture::clone(glm::uvec3 s) {
    auto clone = std::shared_ptr<Texture>(new Texture());

    clone->texture = std::unique_ptr<ExtensionTexture>(texture->clone());
    clone->texture->generateId();

    clone->path = path;
    clone->size = s;
    clone->internal_format = internal_format;
    clone->data_type = data_type;
    clone->format = format;
    clone->target = target;
    clone->min = min;
    clone->mag = mag;
    clone->wrap_r = wrap_r;
    clone->wrap_s = wrap_s;
    clone->wrap_t = wrap_t;
    clone->levels = levels;
    clone->anisotropic = anisotropic;
    clone->mipmap = mipmap;
    clone->compressed = compressed;
    clone->immutable = immutable;

    if (isMutable()) {
        clone->image();
    }

    if (isImmutable()) {
        clone->storage();
    }

    return clone;
}

void Texture::resize(glm::uvec3 _size) {
    size = _size;

    if (isMutable()) {
        image();
    }

    if (isImmutable()) {
        texture = std::unique_ptr<ExtensionTexture>(texture->clone());
        texture->generateId();
        storage();
    }
}

std::shared_ptr<Texture> Texture::clone(glm::uvec2 s) {
    return clone(glm::uvec3{s, 1});
}

void Texture::accept(TextureVisitor& visitor) {
    texture->accept(visitor);
}

Texture& Texture::setMinFilter(Filter filter) {
    min = filter;
    texture->setMinFilter(static_cast<GLenum>(target), static_cast<GLenum>(filter));
    return *this;
}

Texture& Texture::setMagFilter(Filter filter) {
    mag = filter;
    texture->setMagFilter(static_cast<GLenum>(target), static_cast<GLenum>(filter));
    return *this;
}

Texture& Texture::setAnisotropicFilter(float value) {
    anisotropic = value;
    texture->setAnisotropicFilter(static_cast<GLenum>(target), value);
    return *this;
}

Texture& Texture::setAnisotropicFilterMax() {
    anisotropic = ContextInitializer::limits.anisotropic_max;
    setAnisotropicFilter(anisotropic);
    return *this;
}

Texture& Texture::setWrapS(Wrap wrap) {
    wrap_s = wrap;
    texture->setWrapS(static_cast<GLenum>(target), static_cast<GLenum>(wrap_s));
    return *this;
}

Texture& Texture::setWrapT(Wrap wrap) {
    wrap_t = wrap;
    texture->setWrapT(static_cast<GLenum>(target), static_cast<GLenum>(wrap_t));
    return *this;
}

Texture& Texture::setWrapR(Wrap wrap) {
    wrap_r = wrap;
    texture->setWrapR(static_cast<GLenum>(target), static_cast<GLenum>(wrap_r));
    return *this;
}

void Texture::setParameters() {
    setMinFilter(min);
    setMagFilter(mag);
    setWrapS(wrap_s);
    setWrapT(wrap_t);
    setWrapR(wrap_r);

    if (anisotropic != 1.0f) {
        setAnisotropicFilter(anisotropic);
    }
}

uint32_t Texture::getId() const noexcept {
    return texture->getId();
}

bool Texture::isMutable() const noexcept {
    return !immutable;
}

bool Texture::isImmutable() const noexcept {
    return immutable;
}

Texture::Builder Texture::builder() {
    return {};
}

void Texture::copy(std::shared_ptr<Texture> origin, size_t layer, std::shared_ptr<Texture> destination) {
    auto from = Framebuffer::fromTexture(std::move(origin));
    auto to = Framebuffer::fromTexture(std::move(destination), layer);

    to.blit(from, Filter::Nearest);
}
