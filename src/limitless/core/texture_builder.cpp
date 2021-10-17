#include <limitless/core/texture_builder.hpp>

#include <limitless/core/context_initializer.hpp>
#include <limitless/core/named_texture.hpp>
#include <limitless/core/bindless_texture.hpp>

using namespace Limitless;

void TextureBuilder::create() {
    // uses protected destructor
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

void TextureBuilder::createExtensionTexture() {
    ExtensionTexture* extension = ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access") ?
        new NamedTexture(static_cast<GLenum>(texture->target)) : new StateTexture();

    auto* ext = ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture") ?
        new BindlessTexture(extension) : extension;

    texture->texture = std::unique_ptr<ExtensionTexture>(ext);
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
    texture->size = { size, 0 };
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
    createExtensionTexture();

    if (isCompressed()) {
        texture->compressedImage(data, byte_count);
    } else {
        if (isCubeMap()) {
            texture->image(cube_data);
        } else {
            texture->image(data);
        }
    }

    auto tex = std::move(texture);
    create();
    return tex;
}

std::shared_ptr<Texture> TextureBuilder::buildImmutable() {
    createExtensionTexture();

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


