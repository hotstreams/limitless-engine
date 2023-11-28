#include "effects_demoscene.hpp"

#include <limitless/assets.hpp>
#include <limitless/loaders/texture_loader.hpp>
#include <limitless/ms/material_builder.hpp>
#include <limitless/fx/effect_builder.hpp>
#include <limitless/fx/emitters/beam_emitter.hpp>
#include <limitless/fx/emitters/sprite_emitter.hpp>
#include <limitless/fx/emitters/mesh_emitter.hpp>
#include <limitless/loaders/model_loader.hpp>
#include <limitless/instances/model_instance.hpp>
#include <limitless/models/skeletal_model.hpp>
#include <limitless/fx/modules/mesh_location_attachment.hpp>
#include <limitless/instances/instanced_instance.hpp>

using namespace LimitlessDemo;
using namespace Limitless;
using namespace Limitless::ms;
using namespace Limitless::fx;

void EffectsScene::addInstances(Limitless::Assets& assets) {
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

    auto bob = Instance::builder()
                        .model(assets.models.at("bob"))
                        .position({29.0f, 1.0f, 19.0f})
                        .rotation(glm::vec3{0.0f, -M_PI_2, M_PI})
                        .scale(glm::vec3(0.025f))
                        .asSkeletal();
    bob->play("");
    scene.add(bob);

    auto mod = Instance::builder()
                        .effect(assets.effects.at("modeldrop"))
                        .position({0.0f, 0.0f, 0.0f})
                        .asEffect();

    const auto& module = mod->get<fx::SpriteEmitter>("sparks")
        .getModule(fx::ModuleType::InitialMeshLocation);
        dynamic_cast<fx::InitialMeshLocation<fx::SpriteParticle>&>(*module)
        .attachModelInstance(bob.get());
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

EffectsScene::EffectsScene(Limitless::Context& ctx, Limitless::Assets& assets)
    : scene(ctx) {
    addInstances(assets);

    scene.setSkybox(assets.skyboxes.at("skybox"));

    scene.getLighting().getAmbientColor().a *= 0.5;

    scene.add(Light::builder()
         .color({0.6, 1.0, 0.3, 1.0f})
         .direction(glm::vec3{-1.0f})
         .build()
    );

    auto floor = std::make_shared<Limitless::InstancedInstance<Limitless::ModelInstance>>(glm::vec3{0.0f});
    for (int i = 0; i < 30; ++i) {
        for (int j = 0; j < 30; ++j) {
            floor->addInstance(
                    std::make_unique<ModelInstance>(
                            assets.models.at("plane"),
                            assets.materials.at("basic4"),
                            glm::vec3{i, 0.0f, j}
                    )
            );
        }
    }
    scene.add(floor);
}

void EffectsScene::update(Context& context, const Camera& camera) {
    scene.update(context, camera);

    hurricane->rotateBy(glm::vec3(0.0f, 0.3f, 0.0f));
}
