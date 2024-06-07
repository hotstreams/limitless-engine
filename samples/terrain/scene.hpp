#pragma once

#include <limitless/scene.hpp>
#include <limitless/assets.hpp>
#include <limitless/instances/model_instance.hpp>

namespace LimitlessMaterials {
    class Scene {
    private:
        Limitless::Assets& assets;
        Limitless::Scene scene;

        void createTerrain();
    public:
        Scene(Limitless::Context& ctx, Limitless::Assets& assets);

        auto& getScene() { return scene; }

        void update(Limitless::Context& context, const Limitless::Camera& camera);
    };
}