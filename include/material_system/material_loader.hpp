#pragma once

#include <string_view>
#include <memory>

namespace LimitlessEngine {
    class Material;

    class MaterialLoader {
    public:
        std::shared_ptr<Material> load(std::string_view asset_name);
        void save(std::string_view asset_name);
    };
}