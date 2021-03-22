#pragma once

#include <limitless/core/texture_builder.hpp>
#include <limitless/util/filesystem.hpp>
#include <mutex>

namespace LimitlessEngine {
    class TextureLoader {
    public:
        static std::shared_ptr<Texture> load(const fs::path& path, bool bottom_left_start = true);
        static std::shared_ptr<Texture> loadCubemap(const fs::path& path, bool bottom_left_start = false);

        static GLFWimage loadGLFWImage(const fs::path& path);
    };
}