#pragma once

#include <memory>
#include <limitless/util/filesystem.hpp>

namespace Limitless::ms {
    class Material;
}

namespace Limitless {
    class Context;
    class Assets;

    class MaterialLoader {
    public:
        static std::shared_ptr<ms::Material> load(Context& context, Assets& ctx, const fs::path& path);
        static void save(const fs::path& path, const std::shared_ptr<ms::Material>& asset_name);
    };
}