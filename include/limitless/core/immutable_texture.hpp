#pragma once

#include <limitless/core/extension_texture.hpp>
#include <limitless/core/texture.hpp>
#include <memory>

namespace LimitlessEngine {
    class ImmutableTexture : public Texture {
    private:
        std::unique_ptr<ExtensionTexture> texture;
        InternalFormat internal_format;
        texture_parameters params;
        DataType data_type;
        //todo: ?
        [[maybe_unused]] uint8_t samples {};
        GLsizei levels {};
        glm::uvec3 size;
        Format format;
        Type target;
        bool mipmap {};

        void texStorage2D(GLenum target, GLsizei levels, InternalFormat internal_format, glm::uvec2 size) const noexcept;
        void texStorage3D(GLenum target, GLsizei levels, InternalFormat internal_format, glm::uvec3 size) const noexcept;
        void texStorage2DMultiSample(GLenum target, uint8_t samples, InternalFormat internal_format, glm::uvec2 size) const noexcept;
    public:
        // 2D texture construction
        ImmutableTexture(std::unique_ptr<ExtensionTexture> texture, Type target, GLsizei levels, InternalFormat internal_format, glm::uvec2 size, Format format, DataType data_type, const void* data, const texture_parameters& params) noexcept;
        // 3D texture / 2D texture array / empty cubemap array construction
        ImmutableTexture(std::unique_ptr<ExtensionTexture> texture, Type target, GLsizei levels, InternalFormat internal_format, glm::uvec3 size, Format format, DataType data_type, const void* data, const texture_parameters& params) noexcept;
        // cubemap construction
        ImmutableTexture(std::unique_ptr<ExtensionTexture> texture, Type target, GLsizei levels, InternalFormat internal_format, glm::uvec2 size, Format format, DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params) noexcept;

        ImmutableTexture(const ImmutableTexture&) = delete;
        ImmutableTexture& operator=(const ImmutableTexture&) = delete;

        ImmutableTexture(ImmutableTexture&&) noexcept = default;
        ImmutableTexture& operator=(ImmutableTexture&&) noexcept = default;

        void texSubImage2D(glm::uvec2 offset, glm::uvec2 size, const void *data) const noexcept override;
        void texSubImage3D(glm::uvec3 offset, glm::uvec3 size, const void *data) const noexcept override;

        void bind(GLuint index) const noexcept override;
        void generateMipMap() noexcept override;
        void resize(glm::uvec3 size) noexcept override;

        ImmutableTexture& operator<<(const TexParameter<GLint>& param) noexcept override;
        ImmutableTexture& operator<<(const TexParameter<GLfloat>& param) noexcept override;
        ImmutableTexture& operator<<(const TexParameter<GLint*>& param) noexcept override;
        ImmutableTexture& operator<<(const TexParameter<GLfloat*>& param) noexcept override;

        [[nodiscard]] GLuint getId() const noexcept override;
        [[nodiscard]] Type getType() const noexcept override;
        [[nodiscard]] glm::uvec3 getSize() const noexcept override;
        [[nodiscard]] ExtensionTexture& getExtensionTexture() noexcept override;

        void accept(TextureVisitor& visitor) noexcept override;
    };
}