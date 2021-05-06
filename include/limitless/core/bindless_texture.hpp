#pragma once

#include <limitless/core/extension_texture.hpp>
#include <limitless/core/texture_visitor.hpp>
#include <limitless/core/context_state.hpp>
#include <memory>

namespace Limitless {
    class BindlessTexture : public ExtensionTexture {
    private:
        std::unique_ptr<ExtensionTexture> texture;
        GLuint64 handle {};

        void makeBindless() noexcept;
    public:
        explicit BindlessTexture(ExtensionTexture* texture);
        ~BindlessTexture() override;

        BindlessTexture(const BindlessTexture&) = delete;
        BindlessTexture& operator=(const BindlessTexture&) = delete;

        BindlessTexture(BindlessTexture&&) noexcept;
        BindlessTexture& operator=(BindlessTexture&&) noexcept;

        [[nodiscard]] const auto& getHandle() const noexcept { return handle; }

        void makeResident() noexcept;
        void makeNonresident() noexcept;

        // clones extension texture; returns new generated one
        [[nodiscard]] BindlessTexture* clone() const override;

        // immutable storage allocation interface
        void texStorage2D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec2 size) noexcept override;
        void texStorage3D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec3 size) noexcept override;
        void texStorage2DMultisample(GLenum target, uint8_t samples, GLenum internal_format, glm::uvec2 size) noexcept override;

        // mutable storage allocation&loading interface
        void texImage2D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec2 size, const void* data) noexcept override;
        void texImage3D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec3 size, const void *data) noexcept override;
        void texImage2DMultiSample(GLenum target, uint8_t samples, GLenum internal_format, glm::uvec3 size) noexcept override;

        // common loading interface
        void texSubImage2D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, glm::uvec2 size, GLenum format, GLenum type, const void* data) noexcept override;
        void texSubImage3D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, GLenum format, GLenum type, const void* data) noexcept override;

        // mipmap generation
        void generateMipMap(GLenum target) noexcept override;

        // binds texture to specified index
        void bind(GLenum target, GLuint index) const noexcept override;

        // sets various of parameters
        void texParameter(GLenum target, GLenum name, GLint param) noexcept override;
        void texParameter(GLenum target, GLenum name, GLfloat param) noexcept override;
        void texParameter(GLenum target, GLenum name, GLint* params) noexcept override;
        void texParameter(GLenum target, GLenum name, GLfloat* params) noexcept override;

        void accept(TextureVisitor& visitor) noexcept override;

        [[nodiscard]] GLuint getId() const noexcept override { return texture->getId(); };
    };
}