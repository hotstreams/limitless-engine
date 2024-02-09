#pragma once

#include <limitless/assets.hpp>
#include <limitless/renderer/renderer.hpp>

namespace LimitlessMaterials {
    class Assets : public Limitless::Assets {
    private:
        void setUpEffects();
        void setUpCommon(Limitless::Context &ctx);
    public:
        Assets(Limitless::Context& ctx, const fs::path& path);
    };
}