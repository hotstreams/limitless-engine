#pragma once

#include <limitless/core/context_debug.hpp>
#include <limitless/util/filesystem.hpp>
#include <set>

namespace Limitless {
    class Assets;
    class Texture;
    class TextureBuilder;

    enum class TextureLoaderFlag {
        BottomLeftOrigin,
        TopLeftOrigin,
        LinearFilter,
        NearestFilter,
        WrapRepeat,
        WrapClamp,
        MipMap
    };

    using TextureLoaderFlags = std::set<TextureLoaderFlag>;

    class TextureLoader final {
    private:
        static void setTextureParameters(TextureBuilder& builder, const TextureLoaderFlags& flags);
        TextureLoader() = default;
        ~TextureLoader() = default;
    public:
        static inline const auto DEFAULT_LOADING_FLAGS = TextureLoaderFlags {
            TextureLoaderFlag::BottomLeftOrigin,
            TextureLoaderFlag::LinearFilter,
            TextureLoaderFlag::MipMap,
            TextureLoaderFlag::WrapRepeat
        };

        static std::shared_ptr<Texture> load(Assets& assets, const fs::path& path, const TextureLoaderFlags& flags = DEFAULT_LOADING_FLAGS);
        static std::shared_ptr<Texture> loadCubemap(Assets& assets, const fs::path& path, const TextureLoaderFlags& flags = DEFAULT_LOADING_FLAGS);

        static GLFWimage loadGLFWImage(Assets& assets, const fs::path& path, const TextureLoaderFlags& flags = DEFAULT_LOADING_FLAGS);
    };
}