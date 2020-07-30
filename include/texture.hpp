#pragma once

#include <context_debug.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <buffer.hpp>

namespace GraphicsEngine {
    template<typename T>
    struct TexParameter {
        GLenum name;
        T param;
    };

    class Texture {
    public:
        enum class Type {
            Tex2D = GL_TEXTURE_2D,
            Tex3D = GL_TEXTURE_3D,
            CubeMap = GL_TEXTURE_CUBE_MAP,
            Tex2DArray = GL_TEXTURE_2D_ARRAY,
            TexCubeMapArray = GL_TEXTURE_CUBE_MAP_ARRAY,
            Tex2DMS = GL_TEXTURE_2D_MULTISAMPLE
        };

        enum class Wrap {
            Clamp = GL_CLAMP_TO_EDGE,
            Mirrored = GL_MIRRORED_REPEAT,
            Reapeat = GL_REPEAT
        };

        enum class MagFilter {
            Nearest = GL_NEAREST,
            Linear = GL_LINEAR
        };

        enum class MinFilter {
            Nearest = GL_NEAREST,
            Linear = GL_LINEAR,
            NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
            LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
            NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
            LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
        };

        enum class InternalFormat {
            Depth = GL_DEPTH_COMPONENT,
            DepthStencil = GL_DEPTH_STENCIL,
            R = GL_RED,
            RG = GL_RG,
            RGB = GL_RGB,
            RGBA = GL_RGBA,
            R8 = GL_R8,
            RG8 = GL_RG8,
            RGB8 = GL_RGB8,
            RGBA8 = GL_RGBA8
        };

        enum class Format {
            DepthComponent = GL_DEPTH_COMPONENT,
            StencilIndex = GL_STENCIL_INDEX,
            DepthStencil = GL_DEPTH_STENCIL,
            Red = GL_RED,
            Green = GL_GREEN,
            Blue = GL_BLUE,
            RG = GL_RG,
            RGB = GL_RGB,
            RedInt = GL_RED_INTEGER,
            GreenInt = GL_GREEN_INTEGER,
            BlueInt = GL_BLUE_INTEGER,
            RGInt = GL_RG_INTEGER,
            RGBInt = GL_RGB_INTEGER,
            RGBA = GL_RGBA
        };

        enum class DataType {
            UnsignedByte = GL_UNSIGNED_BYTE,
            Byte = GL_BYTE,
            UnsignedShort = GL_UNSIGNED_SHORT,
            Short = GL_SHORT,
            UnsignedInt = GL_UNSIGNED_INT,
            Int = GL_INT,
            Float = GL_FLOAT
        };

        Texture() = default;
        virtual ~Texture() = default;

        virtual void bind(GLuint index) const noexcept = 0;
        virtual void resize(glm::uvec3 size) = 0;

        virtual void texSubImage2D(GLint xoffset, GLint yoffset, glm::uvec2 size, const void* data) const noexcept = 0;
        virtual void texSubImage3D(GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, const void* data) const noexcept = 0;

        virtual void generateMipMap() const noexcept = 0;

        virtual Texture& operator<<(const TexParameter<GLint>& param) noexcept = 0;
        virtual Texture& operator<<(const TexParameter<GLfloat>& param) noexcept = 0;
        virtual Texture& operator<<(const TexParameter<GLint*>& param) noexcept = 0;
        virtual Texture& operator<<(const TexParameter<GLfloat*>& param) noexcept = 0;

        [[nodiscard]] virtual GLuint getId() const noexcept = 0;
        [[nodiscard]] virtual Type getType() const noexcept = 0;
        [[nodiscard]] virtual glm::uvec3 getSize() const noexcept = 0;
    };

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
        StateTexture(Type target, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const void* data);
        // constructor for 3d texture / 2d texture array / empty cubemap array
        StateTexture(Type target, InternalFormat internal, glm::uvec3 size, Format format, DataType data_type, const void* data);
        // constructor for cubemap texture
        StateTexture(Type target, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const std::array<void*, 6>& data);

        // immutable storage constructors
        // constructor for simple 2d texture
        StateTexture(Type target, GLsizei levels, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const void* data);
        // constructor for 3d texture / 2d texture array / empty cubemap array
        StateTexture(Type target, GLsizei levels, InternalFormat internal, glm::uvec3 size, Format format, DataType data_type, const void* data);
        // constructor for cubemap texture
        StateTexture(Type target, GLsizei levels, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const std::array<void*, 6>& data);

        // constructor for multisampled 2d texture
        StateTexture(Type target, uint8_t samples, InternalFormat internal, glm::uvec2 size, bool immutable);

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
    };

    class NamedTexture : public StateTexture {
    private:
        void texStorage2D(GLsizei levels, InternalFormat internal) const noexcept;
        void texStorage3D(GLsizei levels, InternalFormat internal, glm::uvec3 size) const noexcept;
        void texStorage2DMultisample(uint8_t samples, InternalFormat internal) const noexcept override;
    public:
        // mutable storage constructors
        // constructor for simple 2d texture
        NamedTexture(Type target, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const void* data = nullptr);
        // constructor for 3d texture / 2d texture array / empty cubemap array
        NamedTexture(Type target, InternalFormat internal, glm::uvec3 size, Format format, DataType data_type, const void* data = nullptr);
        // constructor for cubemap texture
        NamedTexture(Type target, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const std::array<void*, 6>& data = {nullptr });

        // immutable storage constructors
        // constructor for simple 2d texture
        NamedTexture(Type target, GLsizei levels, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const void* data = nullptr);
        // constructor for 3d texture / 2d texture array / empty cubemap array
        NamedTexture(Type target, GLsizei levels, InternalFormat internal, glm::uvec3 size, Format format, DataType data_type, const void* data = nullptr);
        // constructor for cubemap texture
        NamedTexture(Type target, GLsizei levels, InternalFormat internal, glm::uvec2 size, Format format, DataType data_type, const std::array<void*, 6>& data = {nullptr });

        // constructor for multisampled 2d texture
        NamedTexture(Type target, uint8_t samples, InternalFormat internal, glm::uvec2 size, bool immutable = false);

        ~NamedTexture() override = default;

        void texSubImage2D(GLint xoffset, GLint yoffset, glm::uvec2 size, const void* data) const noexcept override;
        void texSubImage3D(GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, const void* data) const noexcept override;

        void generateMipMap() const noexcept override;

        void bind(GLuint index) const noexcept override;

        NamedTexture& operator<<(const TexParameter<GLint>& param) noexcept override;
        NamedTexture& operator<<(const TexParameter<GLfloat>& param) noexcept override;
        NamedTexture& operator<<(const TexParameter<GLint*>& param) noexcept override;
        NamedTexture& operator<<(const TexParameter<GLfloat*>& param) noexcept override;
    };
}
