#pragma once

#include <texture_builder.hpp>
#include <filesystem>

namespace GraphicsEngine {
    class TextureLoader {
    public:
        static std::shared_ptr<Texture> load(const std::filesystem::path& path, bool bottom_left_start = true);
        static std::shared_ptr<Texture> loadCubemap(const std::filesystem::path& path, bool bottom_left_start = true);

        static GLFWimage loadGLFWImage(const std::filesystem::path& path);
    };
}