#pragma once

#include <limitless/core/extension_texture.hpp>
#include <limitless/core/texture_visitor.hpp>

namespace LimitlessEngine {
    class StateTexture : public ExtensionTexture {
    protected:
        GLuint id {};
    public:
        StateTexture() noexcept;
        ~StateTexture() override;

        StateTexture(const StateTexture&) = delete;
        StateTexture& operator=(const StateTexture&) = delete;

        StateTexture(StateTexture&&) noexcept;
        StateTexture& operator=(StateTexture&&) noexcept;

        [[nodiscard]] StateTexture* clone() const override;

        void texStorage2D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec2 size) noexcept override;
        void texStorage3D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec3 size) noexcept override;
        void texStorage2DMultisample(GLenum target, uint8_t samples, GLenum internal_format, glm::uvec2 size) noexcept override;

        void texImage2D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec2 size, const void* data) noexcept override;
        void texImage3D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec3 size, const void *data) noexcept override;
        void texImage2DMultiSample(GLenum target, uint8_t samples, GLenum internal_format, glm::uvec3 size) noexcept override;

        void texSubImage2D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, glm::uvec2 size, GLenum format, GLenum type, const void* data) noexcept override;
        void texSubImage3D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, GLenum format, GLenum type, const void* data) noexcept override;

        void generateMipMap(GLenum target) noexcept override;

        static void activate(GLuint index);
        void bind(GLenum target, GLuint index) const override;

        void texParameter(GLenum target, GLenum name, GLint param) noexcept override;
        void texParameter(GLenum target, GLenum name, GLfloat param) noexcept override;
        void texParameter(GLenum target, GLenum name, GLint* params) noexcept override;
        void texParameter(GLenum target, GLenum name, GLfloat* params) noexcept override;

        void accept(TextureVisitor& visitor) noexcept override;

        [[nodiscard]] GLuint getId() const noexcept override { return id; }
    };
}