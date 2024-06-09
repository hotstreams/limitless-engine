#pragma once

#include <memory>
#include <limitless/util/filesystem.hpp>

namespace Limitless {
    class EffectInstance;
    class Assets;
    class Context;
    class RendererSettings;

    class EffectLoader {
    public:
        static std::shared_ptr<EffectInstance> load(Assets& assets, const fs::path& path);
        static void save(const fs::path& path, const std::shared_ptr<EffectInstance>& asset);
    };
}