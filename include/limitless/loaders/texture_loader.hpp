#pragma once

#include <limitless/core/texture/texture.hpp>
#include <limitless/core/context_debug.hpp>
#include <limitless/util/filesystem.hpp>
#include <cstdint>
#include <set>
#include <vector>

namespace Limitless {
    class Assets;
    class TextureBuilder;

    class TextureLoaderFlags {
    public:
    	// does not work for DDS formats
        enum class Origin { TopLeft, BottomLeft };
        enum class Filter { Linear, Nearest };
        enum class Compression { None, Default, DXT1, DXT5, BC7, RGTC };
        // works for dds with precomputed mipmaps only
        enum class DownScale { None = 0, x2, x4, x8, x16 };
        enum class Space { sRGB, Linear };

        Origin origin { Origin::BottomLeft };
        Filter filter { Filter::Linear };
        Compression compression { Compression::None };

        DownScale downscale { DownScale::None };
        Texture::Wrap wrapping { Texture::Wrap::Repeat };

        // only for 3 or 4 channels now
        Space space { Space::Linear };

        // for dds it loads mipmaps in a file
        bool mipmap {true};

        // Preserve alpha coverage for masked foliage when using alpha-clip with mipmaps.
        // This adjusts the alpha channel in generated mip levels so that a fixed alpha_cutoff
        // keeps approximately the same coverage at distance (reduces "leaves disappearing").
        // Intended for non-DDS RGBA textures.
        bool preserve_alpha_coverage {false};
        float alpha_coverage_cutoff {0.5f};

        bool anisotropic_filter {false};
        float anisotropic_value {0.0f}; // 0.0f for max supported

        bool border {false};
        glm::vec4 border_color {0.0f};

        TextureLoaderFlags() = default;
        // TextureLoaderFlags(const TextureLoaderFlags&) = default;
        // TextureLoaderFlags(TextureLoaderFlags&&) = default;
        // TextureLoaderFlags& operator=(const TextureLoaderFlags&) = default;
        // TextureLoaderFlags& operator=(TextureLoaderFlags&&) = default;

        TextureLoaderFlags(Origin _origin) noexcept : origin { _origin } {}
        TextureLoaderFlags(Origin _origin, Filter _filter) noexcept : origin { _origin }, filter { _filter } {}
        TextureLoaderFlags(Origin _origin, Space _space) noexcept : origin { _origin }, space {_space} {}
        TextureLoaderFlags(Filter _filter) noexcept : filter { _filter } {}
        TextureLoaderFlags(Filter _filter, Texture::Wrap _wrapping) noexcept : filter { _filter }, wrapping { _wrapping } {}
        TextureLoaderFlags(Space _space) noexcept : space { _space } {}
        TextureLoaderFlags(Texture::Wrap _wrapping) noexcept : wrapping { _wrapping } {}

        TextureLoaderFlags withSpace(Space new_space) const noexcept {
            auto new_flags = *this;
            new_flags.space = new_space;
            return new_flags;
        }

        TextureLoaderFlags withCompression(bool use_compression) const noexcept {
            auto new_flags = *this;
            new_flags.compression = use_compression
                ? TextureLoaderFlags::Compression::Default
                : TextureLoaderFlags::Compression::None;
            return new_flags;
        }

        TextureLoaderFlags withBestCompression() const noexcept {
            return withCompression(true);
        }

        TextureLoaderFlags withNoCompression() const noexcept {
            return withCompression(false);
        }

        TextureLoaderFlags withSrgb() const noexcept {
            return withSpace(Space::sRGB);
        }

        TextureLoaderFlags withLinearSpace() const noexcept {
            return withSpace(Space::Linear);
        }

        TextureLoaderFlags withNoMipmaps() const noexcept {
            auto new_flags = *this;
            new_flags.mipmap = false;
            return new_flags;
        }

        TextureLoaderFlags withPreserveAlphaCoverage(float cutoff) const noexcept {
            auto new_flags = *this;
            new_flags.preserve_alpha_coverage = true;
            new_flags.alpha_coverage_cutoff = cutoff;
            return new_flags;
        }

        TextureLoaderFlags withDownscale(DownScale new_downscale) const noexcept {
            auto new_flags = *this;
            new_flags.downscale = new_downscale;
            return new_flags;
        }

        TextureLoaderFlags withNoDownscale() const noexcept {
            return withDownscale(DownScale::None);
        }

        TextureLoaderFlags withWrapping(Texture::Wrap new_wrapping) const noexcept {
            auto new_flags = *this;
            new_flags.wrapping = new_wrapping;
            return new_flags;
        }

        TextureLoaderFlags withRepeating() const noexcept {
            return withWrapping(Texture::Wrap::Repeat);
        }
    };

    class texture_loader_exception : public std::runtime_error {
    public:
        explicit texture_loader_exception(const char* msg) : std::runtime_error(msg) {}
        explicit texture_loader_exception(std::string msg) : std::runtime_error(std::move(msg)) {}
    };

    /**
     * Raw 8-bit image in memory (stb_image layout, row-major).
     * Used by offline tools (e.g. billboard_generator) without creating GPU textures.
     */
    struct CpuImageBytes {
        int width {0};
        int height {0};
        int channels {0};
        std::vector<uint8_t> data;
    };

    class TextureLoader final {
    private:
        static void setFormat(Texture::Builder& builder, const TextureLoaderFlags& flags, int channels);
        static void setAnisotropicFilter(const std::shared_ptr<Texture>& texture, const TextureLoaderFlags& flags);
        static void setDownScale(int& width, int& height, int channels, unsigned char*& data, const TextureLoaderFlags& flags);
        static bool isPowerOfTwo(int width, int height);
    public:
        TextureLoader() = delete;
        ~TextureLoader() = delete;

        static void setTextureParameters(Texture::Builder& builder, const TextureLoaderFlags& flags);
        static GLFWimage loadGLFWImage(Assets& assets, const fs::path& path, const TextureLoaderFlags& flags = {});

        static std::shared_ptr<Texture> load(Assets& assets, const fs::path& path, const TextureLoaderFlags& flags = {});
        static std::shared_ptr<Texture> load(Assets& assets, const std::vector<fs::path>& paths, const TextureLoaderFlags& flags = {});

        static std::shared_ptr<Texture> load(
            Assets& assets,
            const std::string& name,
            const uint8_t* buffer,
            size_t size,
            const TextureLoaderFlags& flags = {}
        );

        // Expects base path, and will load the 6 cubemap faces with file suffixes: _right, _left, _top, _bottom, _front, _back.
        static std::shared_ptr<Texture> loadCubemap(Assets& assets, const fs::path& path, const TextureLoaderFlags& flags = {});

        // Expects paths for each cubemap face, in the order of Right, Left, Top, Bottom, Front, Back.
        static std::shared_ptr<Texture> loadCubemap(Assets& assets, const std::array<fs::path, 6>& paths, const TextureLoaderFlags& flags = {});

        /**
         * Load image with stb_image (same origin / flip rules as load()).
         * Does not touch Assets or GPU.
         */
        [[nodiscard]] static CpuImageBytes load_image_cpu(const fs::path& path, const TextureLoaderFlags& flags = {});

        [[nodiscard]] static CpuImageBytes load_image_cpu(const uint8_t* buffer, size_t size, const TextureLoaderFlags& flags = {});

        /**
         * Write PNG via stb_image_write (same dependency stack as texture loading).
         * @param stride_bytes row stride; 0 means width * channels.
         */
        [[nodiscard]] static bool save_png(
            const fs::path& path, int width, int height, int channels, const uint8_t* pixels, int stride_bytes = 0
        );

        /**
         * Resize 8-bit image with stb_image_resize (linear filter).
         * @param linear_colorspace STBIR_COLORSPACE_LINEAR for normals, STBIR_COLORSPACE_SRGB for color.
         */
        [[nodiscard]] static CpuImageBytes resize_image_cpu(
            const CpuImageBytes& src, int dst_width, int dst_height, bool linear_colorspace
        );
    };
}