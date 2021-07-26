#pragma once

#include <limitless/core/context_debug.hpp>
#include <limitless/util/filesystem.hpp>
#include <glm/glm.hpp>
#include <functional>
#include <optional>

namespace Limitless {
    class TextureVisitor;
    class ExtensionTexture;

    class Texture {
    public:
        enum class Type {
            Tex2D = GL_TEXTURE_2D,
            Tex3D = GL_TEXTURE_3D,
            CubeMap = GL_TEXTURE_CUBE_MAP,
            Tex2DArray = GL_TEXTURE_2D_ARRAY,
            TexCubeMapArray = GL_TEXTURE_CUBE_MAP_ARRAY
        };

        enum class InternalFormat {
            Depth = GL_DEPTH_COMPONENT,
            Depth16 = GL_DEPTH_COMPONENT16,
            Depth24 = GL_DEPTH_COMPONENT24,
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
            RGBA16F = GL_RGBA16F,
            RGB32F = GL_RGB32F,

            RG8_SNORM = GL_RG8_SNORM,

            RGB8_SNORM = GL_RGB8_SNORM,
            RGBA8_SNORM = GL_RGBA8_SNORM,

            RGB16_SNORM = GL_RGB16_SNORM,
            RGBA16_SNORM = GL_RGBA16_SNORM,

            sRGB8 = GL_SRGB8,
            sRGBA8 = GL_SRGB8_ALPHA8,

            // GL_EXT_texture_compression_s3tc
            RGB_DXT1 = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
            RGBA_DXT1 = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
            RGBA_DXT5 = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,

            sRGB_DXT1 = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,
            sRGBA_DXT1 = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,
            sRGBA_DXT5 = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,

            // GL_ARB_texture_compression_bptc
            RGBA_BC7 = GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,
            sRGBA_BC7 = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB,

            // GL_ARB_texture_compression_rgtc
            R_RGTC = GL_COMPRESSED_RED_RGTC1,
            RG_RGTC = GL_COMPRESSED_RG_RGTC2
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

        enum class Filter {
            Linear = GL_LINEAR,
            Nearest = GL_NEAREST,
            LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR,
            NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
            LinearMipMapNearest = GL_LINEAR_MIPMAP_NEAREST,
            NearestMipMapLinear = GL_NEAREST_MIPMAP_LINEAR
        };

        enum class Wrap {
            Repeat = GL_REPEAT,
            MirroredRepeat = GL_MIRRORED_REPEAT,
            ClampToEdge = GL_CLAMP_TO_EDGE,
            ClampToBorder = GL_CLAMP_TO_BORDER
        };
    private:
        std::optional<fs::path> path;
    public:
        Texture() = default;
        virtual ~Texture() = default;

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&&) noexcept = default;
        Texture& operator=(Texture&&) noexcept = default;

        [[nodiscard]] const auto& getPath() const noexcept { return path; }
        void setPath(fs::path _path) noexcept { path = std::move(_path); }
        
        virtual void bind(GLuint index) const noexcept = 0;

        // resizes texture; content becomes empty
        virtual void resize(glm::uvec3 size) = 0;

        virtual void texSubImage2D(glm::uvec2 offset, glm::uvec2 size, const void *data) const noexcept = 0;
        virtual void texSubImage3D(glm::uvec3 offset, glm::uvec3 size, const void *data) const noexcept = 0;

        virtual void generateMipMap() noexcept = 0;

        virtual Texture& setMinFilter(Filter filter) = 0;
        virtual Texture& setMagFilter(Filter filter) = 0;
        virtual Texture& setAnisotropicFilter(float value) = 0;
        virtual Texture& setAnisotropicFilterMax() = 0;
        virtual Texture& setBorderColor(const glm::vec4& color) = 0;
        virtual Texture& setWrapS(Wrap wrap) = 0;
        virtual Texture& setWrapT(Wrap wrap) = 0;
        virtual Texture& setWrapR(Wrap wrap) = 0;

        [[nodiscard]] virtual GLuint getId() const noexcept = 0;
        [[nodiscard]] virtual Type getType() const noexcept = 0;
        [[nodiscard]] virtual glm::uvec3 getSize() const noexcept = 0;
        [[nodiscard]] virtual ExtensionTexture& getExtensionTexture() noexcept = 0;

        virtual void accept(TextureVisitor& visitor) noexcept = 0;
    };
}
