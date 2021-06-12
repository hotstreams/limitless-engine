#pragma once

#include <limitless/core/texture.hpp>
#include <limitless/core/extension_texture.hpp>

#include <variant>
#include <memory>

namespace Limitless {
    class TextureBuilder {
    private:
        static ExtensionTexture* getSupportedTexture(Texture::Type target);

        Texture::InternalFormat internal {Texture::InternalFormat::RGB};
        Texture::Type target {Texture::Type::Tex2D};
        Texture::Format format {Texture::InternalFormat::RGB};
        Texture::DataType data_type {Texture::DataType::UnsignedByte};

        Texture::Filter min {Texture::Filter::Linear};
        Texture::Filter mag {Texture::Filter::Linear};

        Texture::Wrap wrap_s {Texture::Wrap::Repeat};
        Texture::Wrap wrap_t {Texture::Wrap::Repeat};
        Texture::Wrap wrap_r {Texture::Wrap::Repeat};

        std::variant<const void*, std::array<void*, 6>> data {};

        std::variant<glm::uvec2, glm::uvec3> size {};
        GLsizei levels {1};
        bool mipmap {true};
        bool border {};
        glm::vec4 border_color {0.0f};
    public:
        TextureBuilder& setInternalFormat(Texture::InternalFormat internal);
        TextureBuilder& setDataType(Texture::DataType data_type);
        TextureBuilder& setFormat(Texture::Format format);
        TextureBuilder& setTarget(Texture::Type target);
        TextureBuilder& setData(const void* data);
        TextureBuilder& setData(const std::array<void*, 6>& data);
        TextureBuilder& setLevels(GLsizei levels);
        TextureBuilder& setSize(glm::uvec2 size);
        TextureBuilder& setSize(glm::uvec3 size);
        TextureBuilder& setMipMap(bool mipmap);
        TextureBuilder& setMinFilter(Texture::Filter filter);
        TextureBuilder& setMagFilter(Texture::Filter filter);
        TextureBuilder& setWrapS(Texture::Wrap wrap);
        TextureBuilder& setWrapT(Texture::Wrap wrap);
        TextureBuilder& setWrapR(Texture::Wrap wrap);
        TextureBuilder& setBorder(bool border);
        TextureBuilder& setBorderColor(const glm::vec4& color);

        std::shared_ptr<Texture> buildMutable();

        // builds immutable texture if supported otherwise mutable
        std::shared_ptr<Texture> build();
    };
}
