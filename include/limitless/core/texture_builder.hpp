#pragma once

#include <limitless/core/texture.hpp>
#include <limitless/core/extension_texture.hpp>

#include <variant>
#include <memory>

namespace Limitless {
    /**
     * TextureBuilder is class that is used to build Textures
     * with different properties using different OpenGL extensions
     */
    class TextureBuilder {
    private:
        [[nodiscard]] static bool isImmutable();
        [[nodiscard]] bool isCompressed() const;
        [[nodiscard]] bool isCubeMap() const;
        void create();

        std::unique_ptr<Texture> texture;
        std::array<void*, 6> cube_data {};
        const void* data {};
        std::size_t byte_count {};
    public:
        TextureBuilder();
        ~TextureBuilder() = default;

        TextureBuilder(const TextureBuilder&) = delete;
        TextureBuilder(TextureBuilder&&) = delete;

        TextureBuilder& setInternalFormat(Texture::InternalFormat internal);
        TextureBuilder& setDataType(Texture::DataType data_type);
        TextureBuilder& setFormat(Texture::Format format);
        TextureBuilder& setTarget(Texture::Type target);
        TextureBuilder& setData(const void* data);
        TextureBuilder& setData(const std::array<void*, 6>& data);
        TextureBuilder& setCompressedData(const void* data, std::size_t bytes);
        TextureBuilder& setLevels(uint32_t levels);
        TextureBuilder& setPath(const fs::path& path);
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

        void useStateExtensionTexture();
        void useNamedExtensionTexture();
        void useBindlessExtensionTexture();
        void useBestSupportedExtensionTexture();

        std::shared_ptr<Texture> buildMutable();
        std::shared_ptr<Texture> buildImmutable();
        std::shared_ptr<Texture> build();

        static std::shared_ptr<Texture> asRGBA16NearestClampToEdge(glm::uvec2 size);
        static std::shared_ptr<Texture> asRGB16NearestClampToEdge(glm::uvec2 size);
        static std::shared_ptr<Texture> asRGB16SNORMNearestClampToEdge(glm::uvec2 size);
        static std::shared_ptr<Texture> asRGB16FNearestClampToEdge(glm::uvec2 size);
        static std::shared_ptr<Texture> asDepth32F(glm::uvec2 size);
    };
}
