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
            Depth24Stencil8 = GL_DEPTH24_STENCIL8,
            R = GL_RED,
            R8 = GL_R8,
            RG8 = GL_RG8,
            RGB8 = GL_RGB8,
            RGBA8 = GL_RGBA8,
            RGB16 = GL_RGB16,
            RGBA16 = GL_RGBA16,
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

            RGBA_DXT3 = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,

            sRGB_DXT1 = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,
            sRGBA_DXT1 = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,
	        sRGBA_DXT3 = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,
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
            Float = GL_FLOAT,
            Uint24_8 = GL_UNSIGNED_INT_24_8
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
        std::unique_ptr<ExtensionTexture> texture;
        std::optional<fs::path> path {};
        glm::uvec3 size {1};
        InternalFormat internal_format {InternalFormat::RGB8};
        DataType data_type {DataType::UnsignedByte};
        Format format {Format::RGB};
        Type target {Type::Tex2D};
        Filter min {Filter::Linear};
        Filter mag {Filter::Linear};
        Wrap wrap_r {Wrap::ClampToEdge};
        Wrap wrap_s {Wrap::ClampToEdge};
        Wrap wrap_t {Wrap::ClampToEdge};
        uint32_t levels {1};
        float anisotropic {1.0f};
        bool mipmap {false};
        bool compressed {false};
        bool immutable {false};

        size_t getBytesPerPixel() const noexcept;
    protected:
        Texture() = default;
    public:
        virtual ~Texture() = default;

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&&) noexcept = default;
        Texture& operator=(Texture&&) noexcept = default;

        [[nodiscard]] const auto& getPath() const noexcept { return path; }
        [[nodiscard]] auto getDataType() const noexcept { return data_type; }
        [[nodiscard]] auto getFormat() const noexcept { return format; }
        [[nodiscard]] auto getInternalFormat() const noexcept { return internal_format; }
        [[nodiscard]] auto getType() const noexcept { return target; }
        [[nodiscard]] auto getSize() const noexcept { return size; }
        [[nodiscard]] auto getMin() const noexcept { return min; }
        [[nodiscard]] auto getMag() const noexcept { return mag; }
        [[nodiscard]] auto getWrapR() const noexcept { return wrap_r; }
        [[nodiscard]] auto getWrapS() const noexcept { return wrap_s; }
        [[nodiscard]] auto getWrapT() const noexcept { return wrap_t; }
        [[nodiscard]] auto getLevels() const noexcept { return levels; }
        [[nodiscard]] auto getAnisotropic() const noexcept { return anisotropic; }
        [[nodiscard]] auto hasMipmap() const noexcept { return mipmap; }
        [[nodiscard]] auto isCompressed() const noexcept { return compressed; }
        [[nodiscard]] bool isMutable() const noexcept;
        [[nodiscard]] bool isImmutable() const noexcept;
        [[nodiscard]] auto is2D() const noexcept { return target == Type::Tex2D; }
        [[nodiscard]] auto is3D() const noexcept { return target == Type::Tex3D || target == Type::Tex2DArray; }
        [[nodiscard]] auto isCubemapArray() const noexcept { return target == Type::TexCubeMapArray; }
        [[nodiscard]] uint32_t getId() const noexcept;
        [[nodiscard]] auto& getExtensionTexture() noexcept { return *texture; }
        [[nodiscard]] std::vector<std::byte> getPixels() noexcept;

        Texture& setMinFilter(Filter filter);
        Texture& setMagFilter(Filter filter);
        Texture& setAnisotropicFilter(float value);
        Texture& setAnisotropicFilterMax();
        Texture& setWrapS(Wrap wrap);
        Texture& setWrapT(Wrap wrap);
        Texture& setWrapR(Wrap wrap);
        void setParameters();

        /* ALLOCATION FUNCTIONS */

        // allocates mutable storage
        void image(const void* data = nullptr);
        void image(const std::array<void*, 6>& data);
        void image(const std::vector<const void*>& layers);
        void image(uint32_t level, glm::uvec2 size, const void* data = nullptr);
        void image(uint32_t level, glm::uvec3 size, const void* data = nullptr);

        // allocates immutable storage
        void storage(const void* data = nullptr);
        void storage(const std::array<void*, 6>& data);

        // allocates mutable storage for compressed data
        void compressedImage(const void* data, std::size_t byte_count);
        void compressedImage(uint32_t level, glm::uvec2 size, const void* data, std::size_t byte_count);
        void compressedImage(uint32_t level, glm::uvec3 size, const void* data, std::size_t byte_count);

        /* UPLOADING FUNCTIONS */

        // uploads data
        void subImage(uint32_t level, glm::uvec2 offset, glm::uvec2 size, const void* data);
        void subImage(uint32_t level, glm::uvec3 offset, glm::uvec3 size, const void* data);

        // uploads compressed data
        void compressedSubImage(uint32_t level, glm::uvec2 offset, glm::uvec2 size, const void* data, std::size_t byte_count);
        void compressedSubImage(uint32_t level, glm::uvec3 offset, glm::uvec3 size, const void* data, std::size_t byte_count);

        void generateMipMap();

        void bind(GLuint index) const;

        // resizes texture; content becomes empty
        // TODO: check some strange behavior with framebuffer found: resize + immutable attached textures
        void resize(glm::uvec3 size);

        // copy
        std::shared_ptr<Texture> clone();
        std::shared_ptr<Texture> clone(glm::uvec3 size);
        std::shared_ptr<Texture> clone(glm::uvec2 size);

        void accept(TextureVisitor& visitor);

        class Builder;

        static Builder builder();
    };
}
