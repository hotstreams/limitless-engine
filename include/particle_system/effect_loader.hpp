#pragma once

#include <memory>

namespace LimitlessEngine {
    class EffectInstance;

    class EffectLoader {
    public:
        static std::shared_ptr<EffectInstance> load(const std::string& asset_name);
        static void save(const std::string& asset_name);
    };
}