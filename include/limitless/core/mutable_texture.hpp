#pragma once

#include <limitless/core/texture.hpp>
#include <limitless/core/extension_texture.hpp>
#include <memory>

namespace LimitlessEngine {
    class MutableTexture : public Texture {
    private:
        std::unique_ptr<ExtensionTexture> texture;
        InternalFormat internal_format;
        texture_parameters params;
        DataType data_type;
        glm::uvec3 size;
        Format format;
        Type target;
        bool mipmap {};

        void texImage2D(GLenum target, GLsizei levels, InternalFormat internal_format, Format format, DataType type, glm::uvec2 size, const void* data) const noexcept;
        void texImage3D(GLenum target, GLsizei levels, InternalFormat internal_format, Format format, DataType type, glm::uvec3 size, const void *data) const noexcept;
        void texImage2DMultiSample(uint8_t samples, InternalFormat internal_format, glm::uvec3 size) const noexcept;
    public:
        // 2D texture construction
        MutableTexture(std::unique_ptr<ExtensionTexture> texture, Type target, InternalFormat internal_format, glm::uvec2 size, Format format, DataType data_type, const void* data, const texture_parameters& params) noexcept;
        // 3D texture / 2D texture array / empty cubemap array construction
        MutableTexture(std::unique_ptr<ExtensionTexture> texture, Type target, InternalFormat internal_format, glm::uvec3 size, Format format, DataType data_type, const void* data, const texture_parameters& params) noexcept;
        // cubemap construction
        MutableTexture(std::unique_ptr<ExtensionTexture> texture, Type target, InternalFormat internal_format, glm::uvec2 size, Format format, DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params) noexcept;

        void texSubImage2D(glm::uvec2 offset, glm::uvec2 size, const void *data) const noexcept override;
        void texSubImage3D(glm::uvec3 offset, glm::uvec3 size, const void *data) const noexcept override;

        void bind(GLuint index) const noexcept override;
        void generateMipMap() noexcept override;
        void resize(glm::uvec3 size) noexcept override;

        MutableTexture& operator<<(const TexParameter<GLint>& param) noexcept override;
        MutableTexture& operator<<(const TexParameter<GLfloat>& param) noexcept override;
        MutableTexture& operator<<(const TexParameter<GLint*>& param) noexcept override;
        MutableTexture& operator<<(const TexParameter<GLfloat*>& param) noexcept override;

        [[nodiscard]] GLuint getId() const noexcept override;
        [[nodiscard]] Type getType() const noexcept override;
        [[nodiscard]] glm::uvec3 getSize() const noexcept override;
        [[nodiscard]] const ExtensionTexture& getExtensionTexture() const noexcept override;

        void accept(TextureVisitor& visitor) noexcept override;
    };
}