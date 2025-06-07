#include <limitless/core/texture/texture.hpp>

#include <limitless/core/texture/extension_texture.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/core/context_initializer.hpp>

#include <iostream>

using namespace Limitless;

void Texture::storage(const void* data) {
    setParameters();

    if (is2D()) {
        texture->texStorage2D(static_cast<GLenum>(target), levels, static_cast<GLenum>(internal_format), static_cast<glm::uvec2>(size));
        if (data) {
            subImage(0, {0, 0}, static_cast<glm::uvec2>(size), data);
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

static std::string formatToString(GLenum format) {
    switch (format) {
        case GL_RED: return "GL_RED";
        case GL_RG: return "GL_RG";
        case GL_RGB: return "GL_RGB";
        case GL_RGBA: return "GL_RGBA";
        case GL_DEPTH_COMPONENT: return "GL_DEPTH_COMPONENT";
        case GL_STENCIL_INDEX: return "GL_STENCIL_INDEX";
        case GL_DEPTH_STENCIL: return "GL_DEPTH_STENCIL";
        default: return "Unknown";
    }
}

static std::string dataTypeToString(GLenum dataType) {
    switch (dataType) {
        case GL_UNSIGNED_BYTE: return "GL_UNSIGNED_BYTE";
        case GL_FLOAT: return "GL_FLOAT";
        case GL_INT: return "GL_INT";
        case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
        case GL_SHORT: return "GL_SHORT";
        case GL_UNSIGNED_SHORT: return "GL_UNSIGNED_SHORT";
        case GL_BYTE: return "GL_BYTE";
        case GL_UNSIGNED_INT_24_8: return "GL_UNSIGNED_INT_24_8";
        default: return "Unknown";
    }
}

static std::string targetToString(GLenum target) {
    switch (target) {
        case GL_TEXTURE_2D: return "GL_TEXTURE_2D";
        case GL_TEXTURE_3D: return "GL_TEXTURE_3D";
        case GL_TEXTURE_CUBE_MAP: return "GL_TEXTURE_CUBE_MAP";
        case GL_TEXTURE_CUBE_MAP_ARRAY: return "GL_TEXTURE_CUBE_MAP_ARRAY";
        default: return "Unknown";
    }
}

static std::string bytesToHexString(const std::vector<std::byte>& bytes) {
    std::stringstream ss;
    for (const auto& byte : bytes) {
        ss << std::hex << static_cast<int>(static_cast<unsigned char>(byte)) << " ";
    }
    return ss.str();
}

std::vector<std::byte> Texture::getPixels() noexcept {
    std::vector<std::byte> pixels;
    pixels.resize(size.x * size.y * getBytesPerPixel());

    std::cerr << "getPixels size: " << size.x << " " << size.y << " " << getBytesPerPixel() << std::endl;
    std::cerr << "getPixels format: " << formatToString(static_cast<GLenum>(format)) << " " << dataTypeToString(static_cast<GLenum>(data_type)) << std::endl;
    std::cerr << "getPixels target: " << targetToString(static_cast<GLenum>(target)) << std::endl;
    
    bind(0);
    glGetTexImage(static_cast<GLenum>(target), 
                  0,  // mipmap level 
                  static_cast<GLenum>(format),
                  static_cast<GLenum>(data_type),
                  pixels.data());

    std::cerr << "getPixels pixels: " << bytesToHexString(pixels) << std::endl;
    
    return pixels;
}

size_t Texture::getBytesPerPixel() const noexcept {
    const auto channels = [&]() -> size_t {
        switch (format) {
            case Format::DepthComponent: return 1;
            case Format::StencilIndex: return 1;
            case Format::DepthStencil: return 2;
            case Format::Red: return 1;
            case Format::Green: return 1;
            case Format::Blue: return 1;
            case Format::RG: return 2;
            case Format::RGInt: return 2;
            case Format::RGB: return 3;
            case Format::RGBInt: return 3;
            case Format::RGBA: return 4;
        };
        throw std::runtime_error("Invalid format");
    }();
    
    const auto bytes_per_channel = [&]() -> size_t {
        switch (data_type) {
            case DataType::UnsignedByte: return 1;
            case DataType::Float: return 4;
            case DataType::Int: return 4;
            case DataType::UnsignedInt: return 4;
            case DataType::Short: return 2;
            case DataType::UnsignedShort: return 2;
            case DataType::Byte: return 1;
            case DataType::Uint24_8: return 3;
        };
        throw std::runtime_error("Invalid data type");
    }();
    
    return channels * bytes_per_channel;
}
