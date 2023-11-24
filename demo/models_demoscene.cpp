#include "models_demoscene.hpp"

#include <limitless/instances/model_instance.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/instances/instanced_instance.hpp>
#include <limitless/ms/material.hpp>
#include <random>
#include <iostream>
#include <limitless/loaders/dds_loader.hpp>
#include <limitless/ms/material_builder.hpp>

using namespace LimitlessDemo;
using namespace Limitless;

ModelsScene::ModelsScene(Limitless::Context& ctx, Limitless::Assets& assets)
    : Limitless::Scene(ctx) {
        setSkybox(assets.skyboxes.at("skybox"));

        lighting.add(Light::builder()
             .color({1.0, -1.0, 1.5, 1.0f})
             .direction({-1.0f, -1.0f, -1.0f})
             .build()
        );

        addInstances(assets);
}

void ModelsScene::addInstances(Limitless::Assets& assets) {
    using namespace Limitless;

    auto& floor = add<InstancedInstance<ModelInstance>>(glm::vec3{0.0f});
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

    add<SkeletalInstance>(assets.models.at("thanos"), glm::vec3(25.0f, 1.0f, 1.0f))
            .setRotation(glm::vec3{-M_PI_2, -M_PI_2, 0.0f});

    add<SkeletalInstance>(assets.models.at("daenerys"), glm::vec3(25.0f, 1.0f, 4.0f))
            .setScale(glm::vec3(1.1f))
            .setRotation(glm::vec3{-M_PI_2, -M_PI_2, 0.0f});

    add<SkeletalInstance>(assets.models.at("taskmaster"), glm::vec3(25.0f, 1.0f, 7.0f))
            .setScale(glm::vec3(0.03f))
            .setRotation(glm::vec3{-M_PI_2, -M_PI_2, 0.0f});

    add<SkeletalInstance>(assets.models.at("k2"), glm::vec3(25.0f, 1.0f, 10.0f))
            .setRotation(glm::vec3{-M_PI_2, -M_PI_2, 0.0f});

    add<SkeletalInstance>(assets.models.at("skeleton"), glm::vec3(25.0f, 1.0f, 13.0f))
            .setScale(glm::vec3(0.03f))
            .setRotation(glm::vec3{-M_PI_2, -M_PI_2, 0.0f});

    add<SkeletalInstance>(assets.models.at("bob"), glm::vec3(25.0f, 1.0f, 16.0f))
            .play("")
            .setScale(glm::vec3(0.03f))
            .setRotation(glm::vec3{M_PI, -M_PI_2, 0.0f});

    add<ModelInstance>(assets.models.at("backpack"), glm::vec3(25.0f, 1.5f, 19.0f))
            .setScale(glm::vec3(0.5f))
            .setRotation(glm::vec3{0.0f, -M_PI_2, 0.0f});

    add<ModelInstance>(assets.models.at("cyborg"), glm::vec3(25.0f, 1.0f, 21.0f))
            .setScale(glm::vec3(0.5f))
            .setRotation(glm::vec3{0.0f, -M_PI_2, 0.0f});

    {
        auto& drone = add<ModelInstance>(assets.models.at("drone"), glm::vec3(25.0f, 2.0f, 24.0f))
                .setScale(glm::vec3(0.01f))
                .setRotation(glm::vec3{M_PI, M_PI_2, 0.0f});

        auto it = dynamic_cast<ModelInstance&>(drone).getMeshes().begin();
        for (int i = 0; i < 21; ++i) {
            ++it;
        }
        it->second.hide();
    }

    add<ModelInstance>(assets.models.at("elemental"), glm::vec3(25.0f, 2.0f, 27.0f))
        .setScale(glm::vec3(10.0f))
        .setRotation(glm::vec3{-M_PI_2, -M_PI_2, 0.0f});
}

void ModelsScene::update(Limitless::Context& context, const Limitless::Camera& camera) {
    Limitless::Scene::update(context, camera);
}