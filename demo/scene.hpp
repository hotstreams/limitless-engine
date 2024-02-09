#pragma once

#include <limitless/scene.hpp>
#include "lighting_demoscene.hpp"
#include "../samples/materials_scene/scene.hpp"
#include "effects_demoscene.hpp"
#include "models_demoscene.hpp"

using namespace Limitless;

namespace LimitlessDemo {
    class DemoScene {
    private:
        LightingScene lightingScene;
        MaterialsScene materialsScene;
        EffectsScene effectsScene;
        ModelsScene modelsScene;
        uint32_t current {0};
    public:
        DemoScene(Context& ctx, Assets& assets)
            : lightingScene {ctx, assets}
            , materialsScene {ctx, assets}
            , effectsScene {ctx, assets}
            , modelsScene {ctx, assets} {
        }

        static void setCameraDefault(Camera& camera) {
            camera.setPosition({15.0f, 10.0f, 15.0f});
        }

        void next(Camera& camera) {
            ++current;

            if (current > 4 - 1) {
                current = 0;
            }

            setCameraDefault(camera);
        }

        void update(Context& ctx, Camera& camera) {
            switch (current) {
                case 0:
                    lightingScene.update(ctx, camera);
                case 1:
                    materialsScene.update(ctx, camera);
                case 2:
                    effectsScene.update(ctx, camera);
                case 3:
                    modelsScene.update(ctx, camera);
                default:
                    lightingScene.update(ctx, camera);
            }
        }

        Limitless::Scene& getCurrentScene() noexcept {
            switch (current) {
                case 0:
                    return lightingScene.getScene();
                case 1:
                    return materialsScene.getScene();
                case 2:
                    return effectsScene.getScene();
                case 3:
                    return modelsScene.getScene();
                default:
                    return lightingScene.getScene();
            }
        }
    };
}
