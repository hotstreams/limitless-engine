#pragma once

#include <limitless/scene.hpp>
#include <limitless/assets.hpp>
#include <limitless/instances/model_instance.hpp>

namespace LimitlessMaterials {
    class Scene {
    private:
        Limitless::Assets& assets;
        Limitless::Scene scene;

        // some model with time processing
        std::shared_ptr<Limitless::ModelInstance> open_model;

        void setUpModels();
        void setUpLighting();
    public:
        Scene(Limitless::Context& ctx, Limitless::Assets& assets);

        auto& getScene() { return scene; }

        void update(Limitless::Context& context, const Limitless::Camera& camera);
    };
}