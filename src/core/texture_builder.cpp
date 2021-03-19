#include <core/texture_builder.hpp>

#include <core/context_initializer.hpp>
#include <core/named_texture.hpp>
#include <core/state_texture.hpp>
#include <core/bindless_texture.hpp>
#include <core/mutable_texture.hpp>
#include <core/immutable_texture.hpp>

using namespace LimitlessEngine;

ExtensionTexture* TextureBuilder::getSupportedTexture(Texture::Type target) {
    ExtensionTexture* extension_texture = ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access") ?
                                          new NamedTexture(static_cast<GLenum>(target)) :
                                          new StateTexture();

    return ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture") ?
           new BindlessTexture(extension_texture) :
           extension_texture;
}

std::unique_ptr<Texture> TextureBuilder::build(Texture::Type target, Texture::InternalFormat internal, glm::uvec2 size, Texture::Format format, Texture::DataType data_type, const void* data, const texture_parameters& params) {
    std::unique_ptr<Texture> texture;

    texture = std::make_unique<MutableTexture>(std::unique_ptr<ExtensionTexture>(getSupportedTexture(target)), target, internal, size, format, data_type, data, params);

    return texture;
}

std::shared_ptr<Texture> TextureBuilder::build(Texture::Type target, Texture::InternalFormat internal, glm::uvec3 size, Texture::Format format, Texture::DataType data_type, const void* data, const texture_parameters& params) {
    std::shared_ptr<Texture> texture;

    texture = std::make_shared<MutableTexture>(std::unique_ptr<ExtensionTexture>(getSupportedTexture(target)), target, internal, size, format, data_type, data, params);

    return texture;
}

std::shared_ptr<Texture> TextureBuilder::build(Texture::Type target, Texture::InternalFormat internal, glm::uvec2 size, Texture::Format format, Texture::DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params) {
    std::shared_ptr<Texture> texture;

    texture = std::make_shared<MutableTexture>(std::unique_ptr<ExtensionTexture>(getSupportedTexture(target)), target, internal, size, format, data_type, data, params);

    return texture;
}

std::shared_ptr<Texture> TextureBuilder::build(Texture::Type target, GLsizei levels, Texture::InternalFormat internal, glm::uvec2 size, Texture::Format format, Texture::DataType data_type, const void* data, const texture_parameters& params) {
    std::shared_ptr<Texture> texture;

    auto* extension_texture = getSupportedTexture(target);

    if (ContextInitializer::isExtensionSupported("GL_ARB_texture_storage")) {
        texture = std::make_shared<ImmutableTexture>(std::unique_ptr<ExtensionTexture>(extension_texture), target, levels, internal, size, format, data_type, data, params);
    } else {
        texture = std::make_shared<MutableTexture>(std::unique_ptr<ExtensionTexture>(extension_texture), target, internal, size, format, data_type, data, params);
    }

    return texture;
}

std::shared_ptr<Texture> TextureBuilder::build(Texture::Type target, GLsizei levels, Texture::InternalFormat internal, glm::uvec3 size, Texture::Format format, Texture::DataType data_type, const void* data, const texture_parameters& params) {
    std::shared_ptr<Texture> texture;

    auto* extension_texture = getSupportedTexture(target);

    if (ContextInitializer::isExtensionSupported("GL_ARB_texture_storage")) {
        texture = std::make_shared<ImmutableTexture>(std::unique_ptr<ExtensionTexture>(extension_texture), target, levels, internal, size, format, data_type, data, params);
    } else {
        texture = std::make_shared<MutableTexture>(std::unique_ptr<ExtensionTexture>(extension_texture), target, internal, size, format, data_type, data, params);
    }

    return texture;
}

std::shared_ptr<Texture> TextureBuilder::build(Texture::Type target, GLsizei levels, Texture::InternalFormat internal, glm::uvec2 size, Texture::Format format, Texture::DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params) {
    std::shared_ptr<Texture> texture;

    auto* extension_texture = getSupportedTexture(target);

    if (ContextInitializer::isExtensionSupported("GL_ARB_texture_storage")) {
        texture = std::make_shared<ImmutableTexture>(std::unique_ptr<ExtensionTexture>(extension_texture), target, levels, internal, size, format, data_type, data, params);
    } else {
        texture = std::make_shared<MutableTexture>(std::unique_ptr<ExtensionTexture>(extension_texture), target, internal, size, format, data_type, data, params);
    }

    return texture;
}