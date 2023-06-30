#pragma once

#include <limitless/core/texture/texture.hpp>
#include <limitless/core/texture/extension_texture.hpp>
#include <limitless/core/texture/texture_visitor.hpp>

namespace Limitless {
    class StateTexture : public ExtensionTexture {
    protected:
        GLuint id {};
    public:
        StateTexture() noexcept = default;
        ~StateTexture() override;

        StateTexture(const StateTexture&) = delete;
        StateTexture& operator=(const StateTexture&) = delete;

        StateTexture(StateTexture&&) noexcept;
        StateTexture& operator=(StateTexture&&) noexcept;

        void generateId() noexcept override;

        [[nodiscard]] StateTexture* clone() const override;

        void texStorage2D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec2 size) noexcept override;
        void texStorage3D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec3 size) noexcept override;

        void texImage2D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec2 size, bool border, const void* data) noexcept override;
        void texImage3D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec3 size, bool border, const void *data) noexcept override;

        void compressedTexImage2D(GLenum target, GLint level, GLenum internal_format, glm::uvec2 size, bool border, const void* data, std::size_t bytes) noexcept override;
        void compressedTexImage3D(GLenum target, GLint level, GLenum internal_format, glm::uvec3 size, bool border, const void* data, std::size_t bytes) noexcept override;

        void compressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLenum internal_format, glm::uvec2 size, const void* data, std::size_t bytes) noexcept override;
        void compressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLenum internal_format, glm::uvec3 size, const void* data, std::size_t bytes) noexcept override;

        void texSubImage2D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, glm::uvec2 size, GLenum format, GLenum type, const void* data) noexcept override;
        void texSubImage3D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, GLenum format, GLenum type, const void* data) noexcept override;

        void generateMipMap(GLenum target) noexcept override;

        static void activate(GLuint index);
        void bind(GLenum target, GLuint index) const override;

        StateTexture& setMinFilter(GLenum target, GLenum filter) override;
        StateTexture& setMagFilter(GLenum target, GLenum filter) override;
        StateTexture& setAnisotropicFilter(GLenum target, GLfloat value) override;
        StateTexture& setBorderColor(GLenum target, float* color) override;
        StateTexture& setWrapS(GLenum target, GLenum wrap) override;
        StateTexture& setWrapT(GLenum target, GLenum wrap) override;
        StateTexture& setWrapR(GLenum target, GLenum wrap) override;

        void accept(TextureVisitor& visitor) noexcept override;

        [[nodiscard]] GLuint getId() const noexcept override { return id; }
    };
}