#pragma once

#include <limitless/core/texture/texture.hpp>
#include <limitless/core/texture/extension_texture.hpp>

#include <variant>
#include <memory>

namespace Limitless {
    /**
     * TextureBuilder is class that is used to build Textures
     * with different properties using different OpenGL extensions
     */
    class Texture::Builder final {
    private:
        [[nodiscard]] static bool isImmutable();
        [[nodiscard]] bool isCompressed() const;
        [[nodiscard]] bool isCubeMap() const;
        void create();

        std::unique_ptr<Texture> texture;
        std::array<void*, 6> cube_data {};
        const void* data_ {};
        std::vector<const void*> layers_data;
        std::size_t byte_count {};
    public:
        Builder();

        Builder& internal_format(Texture::InternalFormat internal);
        Builder& data_type(Texture::DataType data_type);
        Builder& format(Texture::Format format);
        Builder& target(Texture::Type target);
        Builder& data(const void* data);
        Builder& layer_data(const void* data);
        Builder& data(const std::array<void*, 6>& data);
        Builder& compressed_data(const void* data, std::size_t bytes);
        Builder& levels(uint32_t levels);
        Builder& path(const fs::path& path);
        Builder& size(glm::uvec2 size);
        Builder& size(glm::uvec3 size);
        Builder& mipmap(bool mipmap);
        Builder& min_filter(Texture::Filter filter);
        Builder& mag_filter(Texture::Filter filter);
        Builder& wrap_s(Texture::Wrap wrap);
        Builder& wrap_t(Texture::Wrap wrap);
        Builder& wrap_r(Texture::Wrap wrap);

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
        static std::shared_ptr<Texture> asArray(const Texture& texture, size_t count);
    };
}
