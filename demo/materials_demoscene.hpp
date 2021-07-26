#pragma once

#include <limitless/scene.hpp>
#include <limitless/assets.hpp>
#include <limitless/instances/model_instance.hpp>

namespace LimitlessDemo {
    class MaterialsScene : public Limitless::Scene {
    private:
        Limitless::ModelInstance* open;
        void addModels(const Limitless::Assets& assets);
    public:
        MaterialsScene(Limitless::Context& ctx, Limitless::Assets& assets);
        ~MaterialsScene() override = default;

        void update(Limitless::Context& context, const Limitless::Camera& camera) override;
    };
}