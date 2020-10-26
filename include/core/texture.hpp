#pragma once

#include <core/texture_visitor.hpp>
#include <core/context_debug.hpp>
#include <glm/glm.hpp>
#include <functional>

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

        enum class InternalFormat {
            Depth = GL_DEPTH_COMPONENT,
            Depth16 = GL_DEPTH_COMPONENT16,
            Depth32 = GL_DEPTH_COMPONENT32,
            Depth32F = GL_DEPTH_COMPONENT32F,
            DepthStencil = GL_DEPTH_STENCIL,
            R = GL_RED,
            RG = GL_RG,
            RGB = GL_RGB,
            RGBA = GL_RGBA,
            R8 = GL_R8,
            RG8 = GL_RG8,
            RGB8 = GL_RGB8,
            RGBA8 = GL_RGBA8,
            RGB16F = GL_RGB16F,
            RGBA16F = GL_RGBA16F
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

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&&) noexcept = default;
        Texture& operator=(Texture&&) noexcept = default;
        
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

        // texture visitors
        virtual void accept(TextureVisitor& visitor) noexcept = 0;
    };

    using texture_parameters = std::function<void(Texture&)>;
}
