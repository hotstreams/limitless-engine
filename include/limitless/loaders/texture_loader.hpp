#pragma once

#include <limitless/core/texture/texture.hpp>
#include <limitless/core/context_debug.hpp>
#include <limitless/util/filesystem.hpp>
#include <set>

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

        bool anisotropic_filter {false};
        float anisotropic_value {0.0f}; // 0.0f for max supported

        bool border {false};
        glm::vec4 border_color {0.0f};

        TextureLoaderFlags() = default;
        TextureLoaderFlags(Origin _origin) noexcept : origin { _origin } {}
        TextureLoaderFlags(Origin _origin, Filter _filter) noexcept : origin { _origin }, filter { _filter } {}
        TextureLoaderFlags(Origin _origin, Space _space) noexcept : origin { _origin }, space {_space} {}
        TextureLoaderFlags(Filter _filter) noexcept : filter { _filter } {}
        TextureLoaderFlags(Filter _filter, Texture::Wrap _wrapping) noexcept : filter { _filter }, wrapping { _wrapping } {}
        TextureLoaderFlags(Space _space) noexcept : space { _space } {}
        TextureLoaderFlags(Texture::Wrap _wrapping) noexcept : wrapping { _wrapping } {}
    };

    class texture_loader_exception : public std::runtime_error {
    public:
        explicit texture_loader_exception(const char* msg) : std::runtime_error(msg) {}
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
        static std::shared_ptr<Texture> loadCubemap(Assets& assets, const fs::path& path, const TextureLoaderFlags& flags = {});
    };
}