#pragma once

#include <limitless/core/context_debug.hpp>
#include "limitless/core/texture/texture_visitor.hpp"
#include <limitless/core/texture/texture.hpp>
#include <glm/glm.hpp>

namespace Limitless {
    class ExtensionTexture {
    public:
        ExtensionTexture() = default;
        virtual ~ExtensionTexture() = default;

        ExtensionTexture(const ExtensionTexture&) = delete;
        ExtensionTexture& operator=(const ExtensionTexture&) = delete;

        ExtensionTexture(ExtensionTexture&&) noexcept = default;
        ExtensionTexture& operator=(ExtensionTexture&&) noexcept = default;

        virtual void generateId() noexcept = 0;

        // clones extension texture; returns new generated one
        [[nodiscard]] virtual ExtensionTexture* clone() const = 0;

        // immutable storage allocation interface
        virtual void texStorage2D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec2 size) noexcept = 0;
        virtual void texStorage3D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec3 size) noexcept = 0;

        // mutable storage allocation&loading interface
        virtual void texImage2D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec2 size, bool border, const void* data) noexcept = 0;
        virtual void texImage3D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec3 size, bool border, const void *data) noexcept = 0;

        // common loading interface
        virtual void texSubImage2D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, glm::uvec2 size, GLenum format, GLenum type, const void* data) noexcept = 0;
        virtual void texSubImage3D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, GLenum format, GLenum type, const void* data) noexcept = 0;

        // compressed texture interface
        virtual void compressedTexImage2D(GLenum target, GLint level, GLenum internal_format, glm::uvec2 size, bool border, const void* data, std::size_t bytes) noexcept = 0;
        virtual void compressedTexImage3D(GLenum target, GLint level, GLenum internal_format, glm::uvec3 size, bool border, const void* data, std::size_t bytes) noexcept = 0;

        virtual void compressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLenum internal_format, glm::uvec2 size, const void* data, std::size_t bytes) noexcept = 0;
        virtual void compressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLenum internal_format, glm::uvec3 size, const void* data, std::size_t bytes) noexcept = 0;

        // mipmap generation
        virtual void generateMipMap(GLenum target) noexcept = 0;

        // binds texture to specified index
        virtual void bind(GLenum target, GLuint index) const = 0;

        virtual ExtensionTexture& setMinFilter(GLenum target, GLenum filter) = 0;
        virtual ExtensionTexture& setMagFilter(GLenum target, GLenum filter) = 0;
        virtual ExtensionTexture& setAnisotropicFilter(GLenum target, GLfloat value) = 0;
        virtual ExtensionTexture& setBorderColor(GLenum target, float* color) = 0;
        virtual ExtensionTexture& setWrapS(GLenum target, GLenum wrap) = 0;
        virtual ExtensionTexture& setWrapT(GLenum target, GLenum wrap) = 0;
        virtual ExtensionTexture& setWrapR(GLenum target, GLenum wrap) = 0;

        virtual void accept(TextureVisitor& visitor) noexcept = 0;

        [[nodiscard]] virtual GLuint getId() const noexcept = 0;
    };
}