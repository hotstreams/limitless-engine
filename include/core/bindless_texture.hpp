#pragma once

#include <core/extension_texture.hpp>
#include <core/texture_visitor.hpp>
#include <memory>

namespace GraphicsEngine {
    class BindlessTexture : public ExtensionTexture {
    private:
        std::unique_ptr<ExtensionTexture> texture;
        mutable GLuint64 handle {};

        void makeBindless() const noexcept;
    public:
        explicit BindlessTexture(ExtensionTexture* texture);
        ~BindlessTexture() override;

        BindlessTexture(const BindlessTexture&) = delete;
        BindlessTexture& operator=(const BindlessTexture&) = delete;

        BindlessTexture(BindlessTexture&&) noexcept;
        BindlessTexture& operator=(BindlessTexture&&) noexcept;

        [[nodiscard]] const auto& getHandle() const noexcept { makeBindless(); return handle; }

        void makeResident() const noexcept;
        void makeNonresident() const noexcept;

        // clones extension texture; returns new generated one
        [[nodiscard]] BindlessTexture* clone() const override;

        // immutable storage allocation interface
        void texStorage2D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec2 size) const noexcept override;
        void texStorage3D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec3 size) const noexcept override;
        void texStorage2DMultisample(GLenum target, uint8_t samples, GLenum internal_format, glm::uvec2 size) const noexcept override;

        // mutable storage allocation&loading interface
        void texImage2D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec2 size, const void* data) const noexcept override;
        void texImage3D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec3 size, const void *data) const noexcept override;
        void texImage2DMultiSample(GLenum target, uint8_t samples, GLenum internal_format, glm::uvec3 size) const noexcept override;

        // common loading interface
        void texSubImage2D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, glm::uvec2 size, GLenum format, GLenum type, const void* data) const noexcept override;
        void texSubImage3D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, GLenum format, GLenum type, const void* data) const noexcept override;

        // mipmap generation
        void generateMipMap(GLenum target) const noexcept override;

        // binds texture to specified index
        void bind(GLenum target, GLuint index) const noexcept override;

        // sets various of parameters
        void texParameter(GLenum target, GLenum name, GLint param) const noexcept override;
        void texParameter(GLenum target, GLenum name, GLfloat param) const noexcept override;
        void texParameter(GLenum target, GLenum name, GLint* params) const noexcept override;
        void texParameter(GLenum target, GLenum name, GLfloat* params) const noexcept override;

        void accept(TextureVisitor& visitor) noexcept override;

        [[nodiscard]] GLuint getId() const noexcept override { return texture->getId(); };
    };
}