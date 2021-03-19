#pragma once

#include <memory>
#include <util/filesystem.hpp>

namespace LimitlessEngine {
    class Material;

    class MaterialLoader {
    public:
        static std::shared_ptr<Material> load(const fs::path& path);
        static void save(const fs::path& path, const std::string& asset_name);
    };
}