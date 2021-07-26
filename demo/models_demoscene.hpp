#pragma once

#include <limitless/scene.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/context.hpp>
#include <limitless/camera.hpp>

namespace LimitlessDemo {
    class ModelsScene : public Limitless::Scene {
    private:
        void addInstances(Limitless::Assets& assets);
    public:
        ModelsScene(Limitless::Context& ctx, Limitless::Assets& assets);
        ~ModelsScene() override = default;

        void update(Limitless::Context& context, const Limitless::Camera& camera) override;
    };
}