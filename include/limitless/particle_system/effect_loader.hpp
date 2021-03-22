#pragma once

#include <memory>
#include <limitless/util/filesystem.hpp>

namespace LimitlessEngine {
    class EffectInstance;

    class EffectLoader {
    public:
        static std::shared_ptr<EffectInstance> load(const fs::path& path);
        static void save(const fs::path& path, const std::string& asset_name);
    };
}