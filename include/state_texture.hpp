#pragma once

#include <texture.hpp>

namespace GraphicsEngine {
    class StateTexture : public Texture {
    protected:
        GLuint id;
        Type target;
        glm::uvec3 size;
        InternalFormat internal;
        Format format;
        DataType data_type;
        bool immutable {false};
        uint8_t samples {0};

        void texStorage2D(GLenum type, GLsizei levels, InternalFormat internal) const noexcept;
        void texStorage3D(GLenum type, GLsizei levels, InternalFormat internal, glm::uvec3 size) const noexcept;
        virtual void texStorage2DMultisample(uint8_t samples, InternalFormat internal) const noexcept;

        void texImage2D(GLenum type, InternalFormat internal, const void* data) const noexcept;
        void texImage3D(GLenum type, InternalFormat internal, glm::uvec3 size, const void* data) const noexcept;
        void texImage2DMultiSample(Type type, uint8_t samples, InternalFormat internal) const noexcept;

        void activate(GLuint index) const noexcept;
        StateTexture() noexcept;
        StateTexture(GLuint id, Type target, glm::uvec3 size, InternalFormat internal, Format format, DataType data_type) noexcept;
    public:
        // mutable storage constructors
        // constructor for simple 2d texture
        StateTexture(Type target, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const void* data, const texture_parameters& params);
        // constructor for 3d texture / 2d texture array / empty cubemap array
        StateTexture(Type target, InternalFormat internal, glm::uvec3 size, Format format, DataType data_type, const void* data, const texture_parameters& params);
        // constructor for cubemap texture
        StateTexture(Type target, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params);

        // immutable storage constructors
        // constructor for simple 2d texture
        StateTexture(Type target, GLsizei levels, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const void* data, const texture_parameters& params);
        // constructor for 3d texture / 2d texture array / empty cubemap array
        StateTexture(Type target, GLsizei levels, InternalFormat internal, glm::uvec3 size, Format format, DataType data_type, const void* data, const texture_parameters& params);
        // constructor for cubemap texture
        StateTexture(Type target, GLsizei levels, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const std::array<void*, 6>& data, const texture_parameters& params);

        // constructor for multisampled 2d texture
        StateTexture(Type target, uint8_t samples, InternalFormat internal, glm::uvec2 size, bool immutable, const texture_parameters& params);

        ~StateTexture() override;

        void texSubImage2D(GLint xoffset, GLint yoffset, glm::uvec2 size, const void* data) const noexcept override;
        void texSubImage3D(GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, const void* data) const noexcept override;

        void generateMipMap() const noexcept override;

        void bind(GLuint index) const noexcept override;
        void resize(glm::uvec3 size) override;

        StateTexture& operator<<(const TexParameter<GLint>& param) noexcept override;
        StateTexture& operator<<(const TexParameter<GLfloat>& param) noexcept override;
        StateTexture& operator<<(const TexParameter<GLint*>& param) noexcept override;
        StateTexture& operator<<(const TexParameter<GLfloat*>& param) noexcept override;

        [[nodiscard]] GLuint getId() const noexcept override;
        [[nodiscard]] Type getType() const noexcept override;
        [[nodiscard]] glm::uvec3 getSize() const noexcept override;

        void accept(TextureVisitor& visitor) const noexcept override;
    };
}