#pragma once

#include <limitless/core/context_debug.hpp>
#include <limitless/util/filesystem.hpp>
#include <set>

namespace Limitless {
    class Assets;
    class Texture;

    enum class TextureLoaderFlag {
        TopLeftOrigin
    };
    using TextureLoaderFlags = std::set<TextureLoaderFlag>;

    class TextureLoader final {
    private:
        Assets& assets;
    public:
        explicit TextureLoader(Assets& _assets) noexcept;
        ~TextureLoader() = default;

        std::shared_ptr<Texture> load(const fs::path& path, const TextureLoaderFlags& flags = {});
        std::shared_ptr<Texture> loadCubemap(const fs::path& path, const TextureLoaderFlags& flags = {});

        GLFWimage loadGLFWImage(const fs::path& path, const TextureLoaderFlags& flags = {});
    };
}