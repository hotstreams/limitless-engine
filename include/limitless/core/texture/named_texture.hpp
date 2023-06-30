#pragma once

#include <limitless/core/texture/state_texture.hpp>
#include "limitless/core/texture/texture_visitor.hpp"

namespace Limitless {
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

        void generateId() noexcept override;

        [[nodiscard]] NamedTexture* clone() const override;

        void texStorage2D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec2 size) noexcept override;
        void texStorage3D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec3 size) noexcept override;

        void texSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, glm::uvec2 size, GLenum format, GLenum type, const void* data) noexcept override;
        void texSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, GLenum format, GLenum type, const void* data) noexcept override;

        void bind(GLenum target, GLuint index) const override;
        void generateMipMap(GLenum target) noexcept override;

        NamedTexture& setMinFilter(GLenum target, GLenum filter) override;
        NamedTexture& setMagFilter(GLenum target, GLenum filter) override;
        NamedTexture& setAnisotropicFilter(GLenum target, GLfloat value) override;
        NamedTexture& setBorderColor(GLenum target, float* color) override;
        NamedTexture& setWrapS(GLenum target, GLenum wrap) override;
        NamedTexture& setWrapT(GLenum target, GLenum wrap) override;
        NamedTexture& setWrapR(GLenum target, GLenum wrap) override;

        void accept(TextureVisitor& visitor) noexcept override;

        [[nodiscard]] GLuint getId() const noexcept override { return id; }
    };
}
