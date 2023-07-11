#include <limitless/core/texture/texture_builder.hpp>

#include <limitless/core/context_initializer.hpp>
#include <limitless/core/texture/named_texture.hpp>
#include <limitless/core/texture/bindless_texture.hpp>
#include <iostream>
#include <memory>

using namespace Limitless;

void TextureBuilder::create() {
    // uses protected ctor
    texture = std::unique_ptr<Texture>(new Texture());
    data = {};
    cube_data = {};
    byte_count = {};
}

bool TextureBuilder::isCompressed() const {
    return byte_count != 0;
}

bool TextureBuilder::isCubeMap() const {
    return cube_data != std::array<void*, 6>{};
}

bool TextureBuilder::isImmutable() {
    return ContextInitializer::isExtensionSupported("GL_ARB_texture_storage");
}

TextureBuilder::TextureBuilder() {
    create();
}

TextureBuilder& TextureBuilder::setInternalFormat(Texture::InternalFormat internal) {
    texture->internal_format = internal;
    return *this;
}

TextureBuilder& TextureBuilder::setFormat(Texture::Format format) {
    texture->format = format;
    return *this;
}

TextureBuilder& TextureBuilder::setTarget(Texture::Type target) {
    texture->target = target;
    return *this;
}

TextureBuilder& TextureBuilder::setDataType(Texture::DataType type) {
    texture->data_type = type;
    return *this;
}

TextureBuilder& TextureBuilder::setData(const void* _data) {
    data = _data;
    return *this;
}

TextureBuilder& TextureBuilder::setData(const std::array<void *, 6>& _data) {
    cube_data = _data;
    return *this;
}

TextureBuilder& TextureBuilder::setSize(glm::uvec2 size) {
    texture->size = { size, 1 };
    return *this;
}

TextureBuilder& TextureBuilder::setSize(glm::uvec3 size) {
    texture->size = size;
    return *this;
}

TextureBuilder& TextureBuilder::setLevels(uint32_t levels) {
    texture->levels = levels;
    return *this;
}

TextureBuilder& TextureBuilder::setPath(const fs::path& path) {
    texture->path = path;
    return *this;
}


TextureBuilder& TextureBuilder::setMipMap(bool mipmap) {
    texture->mipmap = mipmap;
    return *this;
}

TextureBuilder& TextureBuilder::setMinFilter(Texture::Filter filter) {
    texture->min = filter;
    return *this;
}

TextureBuilder& TextureBuilder::setMagFilter(Texture::Filter filter) {
    texture->mag = filter;
    return *this;
}

TextureBuilder& TextureBuilder::setWrapS(Texture::Wrap wrap) {
    texture->wrap_s = wrap;
    return *this;
}

TextureBuilder& TextureBuilder::setWrapT(Texture::Wrap wrap) {
    texture->wrap_t = wrap;
    return *this;
}

TextureBuilder& TextureBuilder::setWrapR(Texture::Wrap wrap) {
    texture->wrap_r = wrap;
    return *this;
}

TextureBuilder& TextureBuilder::setBorder(bool border) {
    texture->border = border;
    return *this;
}

TextureBuilder& TextureBuilder::setBorderColor(const glm::vec4& color) {
    texture->border_color = color;
    return *this;
}

std::shared_ptr<Texture> TextureBuilder::buildMutable() {
    if (!texture->texture) {
        useBestSupportedExtensionTexture();
    }

    if (isCompressed()) {
        texture->compressedImage(data, byte_count);
    } else {
        isCubeMap() ? texture->image(cube_data) : texture->image(data);
    }

    auto tex = std::move(texture);
    create();
    return tex;
}

std::shared_ptr<Texture> TextureBuilder::buildImmutable() {
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
            texture->storage(data);
        }
    }

    auto tex = std::move(texture);
    create();
    tex->bind(0);
    return tex;
}

std::shared_ptr<Texture> TextureBuilder::build() {
    return isImmutable() ? buildImmutable() : buildMutable();
}

TextureBuilder& TextureBuilder::setCompressedData(const void* _data, std::size_t count) {
    data = _data;
    byte_count = count;
    return *this;
}

std::shared_ptr<Texture> TextureBuilder::asRGBA16NearestClampToEdge(glm::uvec2 size) {
    TextureBuilder builder;

    //TODO: change to RGB16 UNSIGNED SHORT?
    builder .setTarget(Texture::Type::Tex2D)
            .setInternalFormat(Texture::InternalFormat::RGBA16)
            .setFormat(Texture::Format::RGBA)
            .setDataType(Texture::DataType::UnsignedByte)
            .setSize(size)
            .setMinFilter(Texture::Filter::Nearest)
            .setMagFilter(Texture::Filter::Nearest)
            .setWrapS(Texture::Wrap::ClampToEdge)
            .setWrapT(Texture::Wrap::ClampToEdge);

    return builder.build();
}

std::shared_ptr<Texture> TextureBuilder::asRGB16NearestClampToEdge(glm::uvec2 size) {
    TextureBuilder builder;

    builder .setTarget(Texture::Type::Tex2D)
            .setInternalFormat(Texture::InternalFormat::RGB16)
            .setFormat(Texture::Format::RGB)
            .setDataType(Texture::DataType::UnsignedByte)
            .setSize(size)
            .setMinFilter(Texture::Filter::Nearest)
            .setMagFilter(Texture::Filter::Nearest)
            .setWrapS(Texture::Wrap::ClampToEdge)
            .setWrapT(Texture::Wrap::ClampToEdge);

    return builder.build();
}

std::shared_ptr<Texture> TextureBuilder::asRGB16SNORMNearestClampToEdge(glm::uvec2 size) {
    TextureBuilder builder;

    builder .setTarget(Texture::Type::Tex2D)
            .setInternalFormat(Texture::InternalFormat::RGB16_SNORM)
            .setFormat(Texture::Format::RGB)
            .setDataType(Texture::DataType::Byte)
            .setSize(size)
            .setMinFilter(Texture::Filter::Nearest)
            .setMagFilter(Texture::Filter::Nearest)
            .setWrapS(Texture::Wrap::ClampToEdge)
            .setWrapT(Texture::Wrap::ClampToEdge);

    return builder.build();
}

std::shared_ptr<Texture> TextureBuilder::asRGB16FNearestClampToEdge(glm::uvec2 size) {
    TextureBuilder builder;

    builder .setTarget(Texture::Type::Tex2D)
            .setInternalFormat(Texture::InternalFormat::RGB16F)
            .setFormat(Texture::Format::RGB)
            .setDataType(Texture::DataType::Float)
            .setSize(size)
            .setMinFilter(Texture::Filter::Nearest)
            .setMagFilter(Texture::Filter::Nearest)
            .setWrapS(Texture::Wrap::ClampToEdge)
            .setWrapT(Texture::Wrap::ClampToEdge)
            .setMipMap(false);

    return builder.build();
}

std::shared_ptr<Texture> TextureBuilder::asDepth32F(glm::uvec2 size) {
    TextureBuilder builder;

    builder .setTarget(Texture::Type::Tex2D)
            .setInternalFormat(Texture::InternalFormat::Depth32F)
            .setFormat(Texture::Format::DepthComponent)
            .setDataType(Texture::DataType::Float)
            .setSize(size)
            .setMinFilter(Texture::Filter::Nearest)
            .setMagFilter(Texture::Filter::Nearest)
            .setWrapS(Texture::Wrap::ClampToEdge)
            .setWrapT(Texture::Wrap::ClampToEdge);

    return builder.build();
}

void TextureBuilder::useStateExtensionTexture() {
    texture->texture = std::make_unique<StateTexture>();
    texture->texture->generateId();
}

void TextureBuilder::useNamedExtensionTexture() {
    if (!ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
        throw std::runtime_error{"NamedExtensionTexture is not supported!"};
    }

    texture->texture = std::make_unique<NamedTexture>(static_cast<GLenum>(texture->target));
    texture->texture->generateId();
}

void TextureBuilder::useBindlessExtensionTexture() {
    if (!ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
        throw std::runtime_error{"BindlessTexture is not supported!"};
    }

    if (!texture->texture) {
        throw std::runtime_error{"ExtensionTexture is not created for wrapping into bindless!"};
    }

    texture->texture = std::make_unique<BindlessTexture>(texture->texture.release());
}

void TextureBuilder::useBestSupportedExtensionTexture() {
    ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access") ? useNamedExtensionTexture() : useStateExtensionTexture();

    if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
        useBindlessExtensionTexture();
    }
}
