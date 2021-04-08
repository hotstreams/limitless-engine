#pragma once

#include <limitless/core/texture.hpp>
#include <limitless/core/extension_texture.hpp>

#include <variant>
#include <memory>

namespace LimitlessEngine {
    class TextureBuilder {
    private:
        static ExtensionTexture* getSupportedTexture(Texture::Type target);

        Texture::InternalFormat internal {Texture::InternalFormat::RGB};
        Texture::Type target {Texture::Type::Tex2D};
        Texture::Format format {Texture::InternalFormat::RGB};
        Texture::DataType data_type {Texture::DataType::UnsignedByte};

        std::variant<const void*, std::array<void*, 6>> data {};

        texture_parameters params {};
        std::variant<glm::uvec2, glm::uvec3> size {};
        GLsizei levels {1};
        bool mipmap {};
    public:
        TextureBuilder& setInternalFormat(Texture::InternalFormat _internal);
        TextureBuilder& setParameters(const texture_parameters& _params);
        TextureBuilder& setDataType(Texture::DataType _data_type);
        TextureBuilder& setFormat(Texture::Format _format);
        TextureBuilder& setTarget(Texture::Type _target);
        TextureBuilder& setData(const void* _data);
        TextureBuilder& setData(const std::array<void*, 6>& _data);
        TextureBuilder& setLevels(GLsizei _levels);
        TextureBuilder& setSize(glm::uvec2 _size);
        TextureBuilder& setSize(glm::uvec3 _size);
        TextureBuilder& setMipMap(bool _mipmap);

        std::shared_ptr<Texture> buildMutable();
        // builds immutable texture if supported
        // otherwise mutable
        std::shared_ptr<Texture> build();
    };
}
