#pragma once

#include <limitless/scene.hpp>
#include <limitless/assets.hpp>
#include <limitless/instances/model_instance.hpp>

namespace LimitlessMaterials {
    class Scene {
    private:
        Limitless::Assets& assets;
        Limitless::Scene scene;

        static constexpr auto FLOOR_INSTANCE_COUNT = 64;
        static constexpr auto LIGHT_COUNT = 512;

        struct light_data {
            glm::vec3 direction {};
            float duration {};
        };
        std::array<light_data, 512> data;

        void setUpModels();
        void setUpLighting();
        bool isInsideFloor(const glm::vec3& position);
    public:
        Scene(Limitless::Context& ctx, Limitless::Assets& assets);

        auto& getScene() { return scene; }

        void update(Limitless::Context& context, const Limitless::Camera& camera);
    };
}