#pragma once

#include <limitless/scene.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/context.hpp>
#include <limitless/camera.hpp>

namespace LimitlessDemo {
    class SponzaScene : public Limitless::Scene {
    private:
    public:
        SponzaScene(Limitless::Context& ctx, Limitless::Assets& assets);
        ~SponzaScene() override = default;

        void update(Limitless::Context& context, const Limitless::Camera& camera) override;
    };
}