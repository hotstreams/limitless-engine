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
    add<EffectInstance>(assets.effects.at("blink"), glm::vec3 {29.0f, 1.0f, 1.0f});
    add<EffectInstance>(assets.effects.at("shield"), glm::vec3{29.0f, 1.0f, 4.0f});
    add<EffectInstance>(assets.effects.at("fireball"), glm::vec3{29.0f, 1.0f, 7.0f});
    add<EffectInstance>(assets.effects.at("explosion"), glm::vec3{29.0f, 1.0f, 10.0f});
    hurricane = &add<EffectInstance>(assets.effects.at("hurricane"), glm::vec3{29.0f, 1.0f, 13.0f});
    add<EffectInstance>(assets.effects.at("lightning"), glm::vec3{29.0f, 1.0f, 16.0f});

    auto& bob = add<SkeletalInstance>(assets.models.at("bob"), glm::vec3(29.0f, 1.0f, 19.0f))
            .play("")
            .setScale(glm::vec3(0.025f))
            .setRotation(glm::vec3{0.0f, -M_PI_2, M_PI});

    const auto& module = add<EffectInstance>(assets.effects.at("modeldrop"), glm::vec3{0.0f})
        .get<fx::SpriteEmitter>("sparks")
        .getModule(fx::ModuleType::InitialMeshLocation);
        dynamic_cast<fx::InitialMeshLocation<fx::SpriteParticle>&>(*module)
        .attachModelInstance(dynamic_cast<SkeletalInstance*>(&bob));

    add<EffectInstance>(assets.effects.at("skeleton"), glm::vec3{23.0f, 1.0f, 1.0f});
    add<EffectInstance>(assets.effects.at("aura"), glm::vec3{23.0f, 1.0f, 4.0f});
}

EffectsScene::EffectsScene(Limitless::Context& ctx, Limitless::Assets& assets)
    : Limitless::Scene(ctx) {
    addInstances(assets);

    setSkybox(assets.skyboxes.at("skybox"));

    lighting.getAmbientColor().a *= 0.5;

    lighting.add(Light::builder()
         .color({2.0, -2.0, 1.5, 1.0f})
         .direction(glm::vec3{-1.0f})
         .build()
    );

    auto& floor = add<Limitless::InstancedInstance<Limitless::ModelInstance>>(glm::vec3{0.0f});
    for (int i = 0; i < 30; ++i) {
        for (int j = 0; j < 30; ++j) {
            floor.addInstance(
                    std::make_unique<ModelInstance>(
                            assets.models.at("plane"),
                            assets.materials.at("basic4"),
                            glm::vec3{i, 0.0f, j}
                    )
            );
        }
    }
}

void EffectsScene::update(Context& context, const Camera& camera) {
    Scene::update(context, camera);

    hurricane->rotateBy(glm::vec3(0.0f, 0.3f, 0.0f));
}
