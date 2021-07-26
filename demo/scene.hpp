#pragma once

#include <limitless/scene.hpp>
#include "lighting_demoscene.hpp"
#include "materials_demoscene.hpp"
#include "effects_demoscene.hpp"
#include "models_demoscene.hpp"
#include "sponza_demoscene.hpp"

using namespace Limitless;

namespace LimitlessDemo {
    class DemoScene {
    private:
        std::map<std::string, std::unique_ptr<Scene>> scenes;
        decltype(scenes)::iterator current;
    public:
        DemoScene(Context& ctx, Assets& assets) {
            scenes.emplace("1. lighting demo", new LightingScene(ctx, assets));
            scenes.emplace("2. materials demo", new MaterialsScene(ctx, assets));
            scenes.emplace("3. effects demo", new EffectsScene(ctx, assets));
            scenes.emplace("4. models demo", new ModelsScene(ctx, assets));
            scenes.emplace("5. sponza demo", new SponzaScene(ctx, assets));

            current = scenes.begin();
        }

        ~DemoScene() = default;

        static void setCameraDefault(Camera& camera) {
            camera.setPosition({15.0f, 10.0f, 15.0f});
        }

        void next(Camera& camera) {
            ++current;

            if (current == scenes.end()) {
                current = scenes.begin();
            }

            setCameraDefault(camera);
        }

        auto getCurrent() noexcept { return current; }
        auto& getCurrentScene() noexcept { return *current->second; }
        [[nodiscard]] const auto& getScenes() const noexcept { return scenes; }
    };
}
