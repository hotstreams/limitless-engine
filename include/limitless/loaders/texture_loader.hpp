#pragma once

#include <limitless/core/texture_builder.hpp>
#include <limitless/util/filesystem.hpp>
#include <mutex>

namespace LimitlessEngine {
    class Assets;

    class TextureLoader {
    private:
        Assets& assets;
    public:
        explicit TextureLoader(Assets& _assets) noexcept : assets {_assets} {}

        std::shared_ptr<Texture> load(const fs::path& path, bool bottom_left_start = true);
        std::shared_ptr<Texture> loadCubemap(const fs::path& path, bool bottom_left_start = false);

        GLFWimage loadGLFWImage(const fs::path& path);
    };
}