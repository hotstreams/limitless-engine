#pragma once

#include <limitless/core/state_texture.hpp>
#include <limitless/core/texture_visitor.hpp>

namespace LimitlessEngine {
    class NamedTexture : public StateTexture {
    private:
        GLenum target {};
    public:
        explicit NamedTexture(GLenum target) noexcept;
        ~NamedTexture() override;

        NamedTexture(const NamedTexture&) = delete;
        NamedTexture& operator=(const NamedTexture&) = delete;

        NamedTexture(NamedTexture&&) noexcept;
        NamedTexture& operator=(NamedTexture&&) noexcept;

        [[nodiscard]] NamedTexture* clone() const override;

        void texStorage2D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec2 size) noexcept override;
        void texStorage3D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec3 size) noexcept override;
        void texStorage2DMultisample(GLenum target, uint8_t samples, GLenum internal_format, glm::uvec2 size) noexcept override;

        void texImage2D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec2 size, const void* data) noexcept override;
        void texImage3D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec3 size, const void *data) noexcept override;
        void texImage2DMultiSample(GLenum target, uint8_t samples, GLenum internal_format, glm::uvec3 size) noexcept override;

        void texSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, glm::uvec2 size, GLenum format, GLenum type, const void* data) noexcept override;
        void texSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, GLenum format, GLenum type, const void* data) noexcept override;

        void bind(GLenum target, GLuint index) const override;
        void generateMipMap(GLenum target) noexcept override;

        void texParameter(GLenum target, GLenum name, GLint param) noexcept override;
        void texParameter(GLenum target, GLenum name, GLfloat param) noexcept override;
        void texParameter(GLenum target, GLenum name, GLint* params) noexcept override;
        void texParameter(GLenum target, GLenum name, GLfloat* params) noexcept override;

        void accept(TextureVisitor& visitor) noexcept override;

        [[nodiscard]] GLuint getId() const noexcept override { return id; }
    };
}
