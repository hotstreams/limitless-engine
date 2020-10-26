#pragma once

#include <core/state_texture.hpp>

namespace GraphicsEngine {
    class NamedTexture : public StateTexture {
    private:
        void texStorage2D(GLsizei levels, InternalFormat internal) const noexcept;
        void texStorage3D(GLsizei levels, InternalFormat internal, glm::uvec3 size) const noexcept;
        void texStorage2DMultisample(uint8_t samples, InternalFormat internal) const noexcept override;
    public:
        // mutable storage constructors
        // constructor for simple 2d texture
        NamedTexture(Type target, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const void* data, const texture_parameters& params);
        // constructor for 3d texture / 2d texture array / empty cubemap array
        NamedTexture(Type target, InternalFormat internal, glm::uvec3 size, Format format, DataType data_type, const void* data, const texture_parameters& params);
        // constructor for cubemap texture
        NamedTexture(Type target, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params);

        // immutable storage constructors
        // constructor for simple 2d texture
        NamedTexture(Type target, GLsizei levels, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const void* data, const texture_parameters& params);
        // constructor for 3d texture / 2d texture array / empty cubemap array
        NamedTexture(Type target, GLsizei levels, InternalFormat internal, glm::uvec3 size, Format format, DataType data_type, const void* data, const texture_parameters& params);
        // constructor for cubemap texture
        NamedTexture(Type target, GLsizei levels, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params);

        // constructor for multisampled 2d texture
        NamedTexture(Type target, uint8_t samples, InternalFormat internal, glm::uvec2 size, bool immutable, const texture_parameters& params);

        ~NamedTexture() override = default;

        void texSubImage2D(GLint xoffset, GLint yoffset, glm::uvec2 size, const void* data) const noexcept override;
        void texSubImage3D(GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, const void* data) const noexcept override;

        void generateMipMap() const noexcept override;

        void bind(GLuint index) const noexcept override;

        NamedTexture& operator<<(const TexParameter<GLint>& param) noexcept override;
        NamedTexture& operator<<(const TexParameter<GLfloat>& param) noexcept override;
        NamedTexture& operator<<(const TexParameter<GLint*>& param) noexcept override;
        NamedTexture& operator<<(const TexParameter<GLfloat*>& param) noexcept override;

        void accept(TextureVisitor& visitor) const noexcept override;
    };
}
