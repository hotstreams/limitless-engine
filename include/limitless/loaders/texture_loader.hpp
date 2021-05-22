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
        Assets& assets;

        static void setTextureParameters(TextureBuilder& builder, const TextureLoaderFlags& flags) ;
    public:
        explicit TextureLoader(Assets& _assets) noexcept;
        ~TextureLoader() = default;

        static inline const auto DEFAULT_LOADING_FLAGS = TextureLoaderFlags {
            TextureLoaderFlag::BottomLeftOrigin,
            TextureLoaderFlag::LinearFilter,
            TextureLoaderFlag::MipMap,
            TextureLoaderFlag::WrapRepeat
        };

        std::shared_ptr<Texture> load(const fs::path& path, const TextureLoaderFlags& flags = DEFAULT_LOADING_FLAGS) const;
        std::shared_ptr<Texture> loadCubemap(const fs::path& path, const TextureLoaderFlags& flags = DEFAULT_LOADING_FLAGS) const;

        GLFWimage loadGLFWImage(const fs::path& path, const TextureLoaderFlags& flags = DEFAULT_LOADING_FLAGS) const;
    };
}