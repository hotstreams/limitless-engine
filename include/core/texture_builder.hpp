#pragma once

#include <core/bindless_texture.hpp>
#include <core/context_initializer.hpp>
#include <core/named_texture.hpp>
#include <core/state_texture.hpp>

namespace GraphicsEngine {
    class TextureBuilder {
    public:
        static std::shared_ptr<Texture> build(Texture::Type target, Texture::InternalFormat internal, glm::uvec2 size, Texture::Format format, Texture::DataType data_type, const void* data, const texture_parameters& params) {
            std::shared_ptr<Texture> texture;

            if (ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
                texture = std::make_shared<NamedTexture>(target, internal, size, format, data_type, data, params);
            } else {
                texture = std::make_shared<StateTexture>(target, internal, size, format, data_type, data, params);
            }

            if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
                return std::shared_ptr<Texture>(new BindlessTexture(std::move(texture)));
            }

            return texture;
        }

        static std::shared_ptr<Texture> build(Texture::Type target, Texture::InternalFormat internal, glm::uvec3 size, Texture::Format format, Texture::DataType data_type, const void* data, const texture_parameters& params) {
            std::shared_ptr<Texture> texture;

            if (ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
                texture = std::make_shared<NamedTexture>(target, internal, size, format, data_type, data, params);
            } else {
                texture = std::make_shared<StateTexture>(target, internal, size, format, data_type, data, params);
            }

            if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
                return std::shared_ptr<Texture>(new BindlessTexture(std::move(texture)));
            }

            return texture;
        }

        static std::shared_ptr<Texture> build(Texture::Type target, Texture::InternalFormat internal, glm::uvec2 size, Texture::Format format, Texture::DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params) {
            std::shared_ptr<Texture> texture;

            if (ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
                texture = std::make_shared<NamedTexture>(target, internal, size, format, data_type, data, params);
            } else {
                texture = std::make_shared<StateTexture>(target, internal, size, format, data_type, data, params);
            }

            if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
                return std::shared_ptr<Texture>(new BindlessTexture(std::move(texture)));
            }

            return texture;
        }

        static std::shared_ptr<Texture> build(Texture::Type target, GLsizei levels, Texture::InternalFormat internal, glm::uvec2 size, Texture::Format format, Texture::DataType data_type, const void* data, const texture_parameters& params) {
            std::shared_ptr<Texture> texture;

            if (ContextInitializer::isExtensionSupported("GL_ARB_texture_storage")) {
                if (ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
                    texture = std::make_shared<NamedTexture>(target, levels, internal, size, format, data_type, data, params);
                } else {
                    texture = std::make_shared<StateTexture>(target, levels, internal, size, format, data_type, data, params);
                }
            } else {
                if (ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
                    texture = std::make_shared<NamedTexture>(target, internal, size, format, data_type, data, params);
                } else {
                    texture = std::make_shared<StateTexture>(target, internal, size, format, data_type, data, params);
                }
            }

            if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
                return std::shared_ptr<Texture>(new BindlessTexture(std::move(texture)));
            }

            return texture;
        }

        static std::shared_ptr<Texture> build(Texture::Type target, GLsizei levels, Texture::InternalFormat internal, glm::uvec3 size, Texture::Format format, Texture::DataType data_type, const void* data, const texture_parameters& params) {
            std::shared_ptr<Texture> texture;

            if (ContextInitializer::isExtensionSupported("GL_ARB_texture_storage")) {
                if (ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
                    texture = std::make_shared<NamedTexture>(target, levels, internal, size, format, data_type, data, params);
                } else {
                    texture = std::make_shared<StateTexture>(target, levels, internal, size, format, data_type, data, params);
                }
            } else {
                if (ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
                    texture = std::make_shared<NamedTexture>(target, internal, size, format, data_type, data, params);
                } else {
                    texture = std::make_shared<StateTexture>(target, internal, size, format, data_type, data, params);
                }
            }

            if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
                return std::shared_ptr<Texture>(new BindlessTexture(std::move(texture)));
            }

            return texture;
        }

        static std::shared_ptr<Texture> build(Texture::Type target, GLsizei levels, Texture::InternalFormat internal, glm::uvec2 size, Texture::Format format, Texture::DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params) {
            std::shared_ptr<Texture> texture;

            if (ContextInitializer::isExtensionSupported("GL_ARB_texture_storage")) {
                if (ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
                    texture = std::make_shared<NamedTexture>(target, levels, internal, size, format, data_type, data, params);
                } else {
                    texture = std::make_shared<StateTexture>(target, levels, internal, size, format, data_type, data, params);
                }
            } else {
                if (ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
                    texture = std::make_shared<NamedTexture>(target, internal, size, format, data_type, data, params);
                } else {
                    texture = std::make_shared<StateTexture>(target, internal, size, format, data_type, data, params);
                }
            }

            if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
                return std::shared_ptr<Texture>(new BindlessTexture(std::move(texture)));
            }

            return texture;
        }

        static std::shared_ptr<Texture> build(Texture::Type target, uint8_t samples, Texture::InternalFormat internal, glm::uvec2 size, bool immutable, const texture_parameters& params) {
            std::shared_ptr<Texture> texture;
            bool imm = immutable & ContextInitializer::isExtensionSupported("GL_ARB_texture_storage");

            if (ContextInitializer::isExtensionSupported("GL_ARB_direct_state_access")) {
                texture = std::make_shared<NamedTexture>(target, samples, internal, size, imm, params);
            } else {
                texture = std::make_shared<StateTexture>(target, samples, internal, size, imm, params);
            }

            if (ContextInitializer::isExtensionSupported("GL_ARB_bindless_texture")) {
                return std::shared_ptr<Texture>(new BindlessTexture(std::move(texture)));
            }

            return texture;
        }
    };
}