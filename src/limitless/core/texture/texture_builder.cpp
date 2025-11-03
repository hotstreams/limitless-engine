#include <limitless/core/texture/texture_builder.hpp>

#include <limitless/core/context_initializer.hpp>
#include <limitless/core/texture/named_texture.hpp>
#include <limitless/core/texture/bindless_texture.hpp>
#include <iostream>
#include <memory>

using namespace Limitless;

void Texture::Builder::create() {
    // uses protected ctor
    texture = std::unique_ptr<Texture>(new Texture());
    data_ = {};
    cube_data = {};
    byte_count = {};
}

bool Texture::Builder::isCompressed() const {
    return byte_count != 0;
}

bool Texture::Builder::isCubeMap() const {
    return cube_data != std::array<void*, 6>{};
}

bool Texture::Builder::isImmutable() {
    return ContextInitializer::isExtensionSupported("GL_ARB_texture_storage");
}

Texture::Builder::Builder() {
    create();
}

Texture::Builder& Texture::Builder::internal_format(Texture::InternalFormat internal) {
    texture->internal_format = internal;
    return *this;
}

Texture::Builder& Texture::Builder::format(Texture::Format format) {
    texture->format = format;
    return *this;
}

Texture::Builder& Texture::Builder::target(Texture::Type target) {
    texture->target = target;
    return *this;
}

Texture::Builder& Texture::Builder::data_type(Texture::DataType type) {
    texture->data_type = type;
    return *this;
}

Texture::Builder& Texture::Builder::data(const void* _data) {
    data_ = _data;
    return *this;
}

Texture::Builder & Texture::Builder::layer_data(const void *data) {
    layers_data.emplace_back(data);
    return *this;
}

Texture::Builder& Texture::Builder::data(const std::array<void *, 6>& _data) {
    cube_data = _data;
    return *this;
}

Texture::Builder& Texture::Builder::size(glm::uvec2 size) {
    texture->size = { size, 1 };
    return *this;
}

Texture::Builder& Texture::Builder::size(glm::uvec3 size) {
    texture->size = size;
    return *this;
}

Texture::Builder& Texture::Builder::levels(uint32_t levels) {
    texture->levels = levels;
    return *this;
}

Texture::Builder& Texture::Builder::path(const fs::path& path) {
    texture->path = path;
    return *this;
}


Texture::Builder& Texture::Builder::mipmap(bool mipmap) {
    texture->mipmap = mipmap;
    return *this;
}

Texture::Builder& Texture::Builder::min_filter(Texture::Filter filter) {
    texture->min = filter;
    return *this;
}

Texture::Builder& Texture::Builder::mag_filter(Texture::Filter filter) {
    texture->mag = filter;
    return *this;
}

Texture::Builder& Texture::Builder::wrap_s(Texture::Wrap wrap) {
    texture->wrap_s = wrap;
    return *this;
}

Texture::Builder& Texture::Builder::wrap_t(Texture::Wrap wrap) {
    texture->wrap_t = wrap;
    return *this;
}

Texture::Builder& Texture::Builder::wrap_r(Texture::Wrap wrap) {
    texture->wrap_r = wrap;
    return *this;
}

std::shared_ptr<Texture> Texture::Builder::buildMutable() {
    if (!texture->texture) {
        useBestSupportedExtensionTexture();
    }

    if (isCompressed()) {
        texture->compressedImage(data_, byte_count);
    } else {
        if (!layers_data.empty()) {
            texture->image(layers_data);
        } else {
            isCubeMap() ? texture->image(cube_data) : texture->image(data_);
        }
    }

    auto tex = std::move(texture);
    create();
    return tex;
}

std::shared_ptr<Texture> Texture::Builder::buildImmutable() {
    if (!texture->texture) {
        useBestSupportedExtensionTexture();
    }

    texture->immutable = true;

    if (!isImmutable()) {
        throw std::runtime_error("Immutable textures not supported!");
    }

    if (isCompressed()) {
        throw std::runtime_error("Compressed immutable textures not yet supported!");
    } else {
        if (isCubeMap()) {
            texture->storage(cube_data);
        } else {
            texture->storage(data_);
        }
    }

    auto tex = std::move(texture);
    create();
    tex->bind(0);
    return tex;
}

std::shared_ptr<Texture> Texture::Builder::build() {
    return isImmutable() ? buildImmutable() : buildMutable();
}

Texture::Builder& Texture::Builder::compressed_data(const void* _data, std::size_t count) {
    data_= _data;
    byte_count = count;
    return *this;
}

std::shared_ptr<Texture> Texture::Builder::asRGBA16NearestClampToEdge(glm::uvec2 size) {
    //TODO: change to RGB16 UNSIGNED SHORT?
    return Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::RGBA16)
            .format(Texture::Format::RGBA)
            .data_type(Texture::DataType::UnsignedByte)
            .size(size)
            .min_filter(Texture::Filter::Nearest)
            .mag_filter(Texture::Filter::Nearest)
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .build();
}

std::shared_ptr<Texture> Texture::Builder::asRGB16NearestClampToEdge(glm::uvec2 size) {
    return Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::RGB16)
            .format(Texture::Format::RGB)
            .data_type(Texture::DataType::UnsignedByte)
            .size(size)
            .min_filter(Texture::Filter::Nearest)
            .mag_filter(Texture::Filter::Nearest)
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .build();
}

std::shared_ptr<Texture> Texture::Builder::asRGB16SNORMNearestClampToEdge(glm::uvec2 size) {
    return Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::RGB16_SNORM)
            .format(Texture::Format::RGB)
            .data_type(Texture::DataType::Byte)
            .size(size)
            .min_filter(Texture::Filter::Nearest)
            .mag_filter(Texture::Filter::Nearest)
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .build();
}

std::shared_ptr<Texture> Texture::Builder::asRGB16FNearestClampToEdge(glm::uvec2 size) {
    return Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::RGB16F)
            .format(Texture::Format::RGB)
            .data_type(Texture::DataType::Float)
            .size(size)
            .min_filter(Texture::Filter::Nearest)
            .mag_filter(Texture::Filter::Nearest)
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .mipmap(false)
            .build();
}

std::shared_ptr<Texture> Texture::Builder::asDepth32F(glm::uvec2 size) {
    return Texture::builder()
            .target(Texture::Type::Tex2D)
            .internal_format(Texture::InternalFormat::Depth32F)
            .format(Texture::Format::DepthComponent)
            .data_type(Texture::DataType::Float)
            .size(size)
            .min_filter(Texture::Filter::Nearest)
            .mag_filter(Texture::Filter::Nearest)
            .wrap_s(Texture::Wrap::ClampToEdge)
            .wrap_t(Texture::Wrap::ClampToEdge)
            .build();
}

void Texture::Builder::useStateExtensionTexture() {
    texture->texture = std::make_unique<StateTexture>();
    texture->texture->generateId();
}

void Texture::Builder::useNamedExtensionTexture() {
    if (!ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
        throw std::runtime_error{"NamedExtensionTexture is not supported!"};
    }

    texture->texture = std::make_unique<NamedTexture>(static_cast<GLenum>(texture->target));
    texture->texture->generateId();
}

void Texture::Builder::useBindlessExtensionTexture() {
    if (!ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
        throw std::runtime_error{"BindlessTexture is not supported!"};
    }

    if (!texture->texture) {
        throw std::runtime_error{"ExtensionTexture is not created for wrapping into bindless!"};
    }

    texture->texture = std::make_unique<BindlessTexture>(texture->texture.release());
}

void Texture::Builder::useBestSupportedExtensionTexture() {
    ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access") ? useNamedExtensionTexture() : useStateExtensionTexture();

    if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
        useBindlessExtensionTexture();
    }
}

std::shared_ptr<Texture> Texture::Builder::asArray(const Texture& texture, size_t count) {
    return
        Texture::builder()
            .size({texture.getSize().x, texture.getSize().y, count})
            .target(Type::Tex2DArray)
            .mag_filter(texture.getMag())
            .min_filter(texture.getMin())
            .data_type(texture.getDataType())
            .format(texture.getFormat())
            .internal_format(texture.getInternalFormat())
            .wrap_s(texture.getWrapS())
            .wrap_t(texture.getWrapT())
            .wrap_r(texture.getWrapR())
            .build();
}
