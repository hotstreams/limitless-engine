#pragma once

#include <limitless/scene.hpp>
#include <limitless/assets.hpp>
#include <limitless/instances/model_instance.hpp>

namespace LimitlessDemo {
    class MaterialsScene {
    private:
        Limitless::Scene scene;
        std::shared_ptr<Limitless::ModelInstance> open;
        void addModels(const Limitless::Assets& assets);
    public:
        MaterialsScene(Limitless::Context& ctx, Limitless::Assets& assets);

        auto& getScene() { return scene; }

        void update(Limitless::Context& context, const Limitless::Camera& camera);
    };
}