#pragma once

#include <memory>
#include <limitless/util/filesystem.hpp>

namespace LimitlessEngine {
    class Material;
    class Assets;

    class MaterialLoader {
    public:
        static std::shared_ptr<Material> load(Assets& ctx, const fs::path& path);
        static void save(const fs::path& path, const std::shared_ptr<Material>& asset_name);
    };
}