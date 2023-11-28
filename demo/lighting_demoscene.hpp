#pragma once

#include <limitless/scene.hpp>
#include <limitless/core/context.hpp>
#include <limitless/camera.hpp>
#include <limitless/assets.hpp>

namespace LimitlessDemo {
    class LightingScene {
    private:
        Limitless::Scene scene;

        static constexpr auto FLOOR_INSTANCE_COUNT = 64;
        static constexpr auto LIGHT_COUNT = 512;

        struct light_data {
            glm::vec3 direction {};
            float duration {};
        };
        std::array<light_data, LIGHT_COUNT> data;

        void addFloor(const Limitless::Assets& assets);
        void addSpheres(const Limitless::Assets& assets);
        static bool isInsideFloor(const glm::vec3& position);
        void addLights();
    public:
        LightingScene(Limitless::Context& ctx, const Limitless::Assets& assets);

        auto& getScene() { return scene; }

        void update(Limitless::Context& context, const Limitless::Camera& camera);
    };
}