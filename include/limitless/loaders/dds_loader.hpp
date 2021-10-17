#pragma once

#include <limitless/assets.hpp>
#include <limitless/loaders/texture_loader.hpp>

namespace Limitless {
    class dds_loader_exception : std::runtime_error {
    public:
        explicit dds_loader_exception(const char* msg) : std::runtime_error(msg) {}
    };

    class DDSLoader {
        static std::size_t getDXTByteCount(glm::uvec2 size, std::size_t block_size) noexcept;

        static void loadLevel(std::shared_ptr<Texture>& texture, std::ifstream& fs, uint32_t level, int channels);
    public:
        static std::shared_ptr<Texture> load(Assets& assets, const fs::path& path, const TextureLoaderFlags& flags);
    };
}
