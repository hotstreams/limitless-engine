#include "scene.hpp"

#include <limitless/scene.hpp>
#include <limitless/instances/instanced_instance.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/lighting/light.hpp>
#include <limitless/fx/modules/mesh_location.hpp>
#include <limitless/fx/particle.hpp>
#include <limitless/fx/emitters/sprite_emitter.hpp>

using namespace LimitlessMaterials;
using namespace Limitless;

LimitlessMaterials::Scene::Scene(Limitless::Context& ctx, Limitless::Assets& assets)
    : scene(ctx)
    , assets {assets} {
    setUpLighting();
    setUpEffects();
}

void LimitlessMaterials::Scene::update(Limitless::Context& context, const Limitless::Camera& camera) {
    scene.update(camera);
    hurricane->rotateBy(glm::vec3(0.0f, 0.3f, 0.0f));
}

void LimitlessMaterials::Scene::setUpEffects() {
    using namespace Limitless;

    auto floor = std::make_shared<InstancedInstance>();
    for (int i = 0; i < 30; ++i) {
        for (int j = 0; j < 30; ++j) {
            floor->add(Instance::builder()
                .model(assets.models.at("plane"))
                .material(assets.materials.at("basic1"))
                .position(glm::vec3{i, 0.0f, j})
                .asModel()
            );
        }
    }
    scene.add(floor);

    scene.add(Instance::builder()
          .effect(assets.effects.at("blink"))
          .position({29.0f, 1.0f, 1.0f})
          .build()
    );

    scene.add(Instance::builder()
          .effect(assets.effects.at("shield"))
          .position({29.0f, 1.0f, 4.0f})
          .build()
    );

    scene.add(Instance::builder()
          .effect(assets.effects.at("fireball"))
          .position({29.0f, 1.0f, 7.0f})
          .build()
    );

    scene.add(Instance::builder()
          .effect(assets.effects.at("explosion"))
          .position({29.0f, 1.0f, 10.0f})
          .build()
    );

    hurricane = Instance::builder()
            .effect(assets.effects.at("hurricane"))
            .position({29.0f, 1.0f, 13.0f})
            .asEffect();
    scene.add(hurricane);

    scene.add(Instance::builder()
          .effect(assets.effects.at("lightning"))
          .position({29.0f, 1.0f, 16.0f})
          .build()
    );

    auto stem = Instance::builder()
        .model(assets.models.at("model"))
        .position({29.0f, 1.0f, 19.0f})
        .asSkeletal();
    stem->play(stem->getAllAnimations()[0].name);
    scene.add(stem);

    auto mod = Instance::builder()
        .effect(assets.effects.at("modeldrop"))
        .position({0.0f, 0.0f, 0.0f})
        .asEffect();

    const auto& module = mod->get<fx::SpriteEmitter>("sparks").getModule(fx::ModuleType::InitialMeshLocation);

    dynamic_cast<fx::InitialMeshLocation<fx::SpriteParticle>&>(*module).attachModelInstance(stem.get());

    scene.add(mod);

    scene.add(Instance::builder()
                      .effect(assets.effects.at("skeleton"))
                      .position({23.0f, 1.0f, 1.0f})
                      .build()
    );

    scene.add(Instance::builder()
                      .effect(assets.effects.at("aura"))
                      .position({23.0f, 1.0f, 4.0f})
                      .build()
    );
}

void LimitlessMaterials::Scene::setUpLighting() {
    scene.getLighting().setAmbientColor(glm::vec4(1.0f));

    scene.setSkybox(assets.skyboxes.at("skybox"));

    scene.add(Light::builder()
          .color(glm::vec4(1.0, 1.0, 1.0, 1.0f))
          .direction(glm::vec3{0.3f, -0.99f, 0.0f})
          .build()
    );
}
