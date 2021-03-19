#pragma once

#include <core/texture.hpp>
#include <core/extension_texture.hpp>

#include <memory>

namespace LimitlessEngine {
   class TextureBuilder {
   private:
        static ExtensionTexture* getSupportedTexture(Texture::Type target);
    public:
        static std::unique_ptr<Texture> build(Texture::Type target, Texture::InternalFormat internal, glm::uvec2 size, Texture::Format format, Texture::DataType data_type, const void* data, const texture_parameters& params);
        static std::shared_ptr<Texture> build(Texture::Type target, Texture::InternalFormat internal, glm::uvec3 size, Texture::Format format, Texture::DataType data_type, const void* data, const texture_parameters& params);
        static std::shared_ptr<Texture> build(Texture::Type target, Texture::InternalFormat internal, glm::uvec2 size, Texture::Format format, Texture::DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params);
        static std::shared_ptr<Texture> build(Texture::Type target, GLsizei levels, Texture::InternalFormat internal, glm::uvec2 size, Texture::Format format, Texture::DataType data_type, const void* data, const texture_parameters& params);
        static std::shared_ptr<Texture> build(Texture::Type target, GLsizei levels, Texture::InternalFormat internal, glm::uvec3 size, Texture::Format format, Texture::DataType data_type, const void* data, const texture_parameters& params);
        static std::shared_ptr<Texture> build(Texture::Type target, GLsizei levels, Texture::InternalFormat internal, glm::uvec2 size, Texture::Format format, Texture::DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params);

//        static std::shared_ptr<Texture> build(Texture::Type target, uint8_t samples, Texture::InternalFormat internal, glm::uvec2 size, bool immutable, const texture_parameters& params) {
//            std::shared_ptr<Texture> texture;
//            bool imm = immutable & ContextInitializer::isExtensionSupported("GL_ARB_texture_storage");
//
//            if (ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
//                texture = std::make_shared<NamedTexture>(target, samples, internal, size, imm, params);
//            } else {
//                texture = std::make_shared<StateTexture>(target, samples, internal, size, imm, params);
//            }
//
//            if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
//                return std::shared_ptr<Texture>(new BindlessTexture(std::move(texture)));
//            }
//
//            return texture;
//        }
    };
}