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

ModelsScene::ModelsScene(Limitless::Context& ctx, Limitless::Assets& assets)
    : Limitless::Scene(ctx) {
        setSkybox(assets.skyboxes.at("skybox"));
        lighting.ambient_color.a = 0.5f;
        lighting.directional_light = {glm::vec4(2.0, -2.0, 1.5, 1.0f), glm::vec4{1.5f, 1.0f, 1.0f, 1.0f}};
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

//    add<SkeletalInstance>(assets.models.at("bob"), glm::vec3(25.0f, 1.0f, 16.0f))
//            .setScale(glm::vec3(0.03f))
//            .setRotation(glm::vec3{M_PI, -M_PI_2, 0.0f})
//            .play("");

    add<ModelInstance>(assets.models.at("backpack"), glm::vec3(25.0f, 1.5f, 19.0f))
            .setScale(glm::vec3(0.5f))
            .setRotation(glm::vec3{0.0f, -M_PI_2, 0.0f});

    add<ModelInstance>(assets.models.at("cyborg"), glm::vec3(25.0f, 1.0f, 21.0f))
            .setScale(glm::vec3(0.5f))
            .setRotation(glm::vec3{0.0f, -M_PI_2, 0.0f});

    add<ModelInstance>(assets.models.at("cube"), assets.materials.at("tesselation_sample"), glm::vec3(25.0f, 2.0f, 30.0f));

//    auto& terrain = add<TerrainInstance>(assets.models.at("planequad"), assets.materials.at("tesselation_sample"), glm::vec3(0.0f, 10.0f, 0.0f), 32);
    auto& terrain = add<ModelInstance>(assets.models.at("planequad"), assets.materials.at("tesselation_sample"), glm::vec3(0.0f, 10.0f, 0.0f))
            .setScale(glm::vec3(32.0f));

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

    add<SkeletalInstance>(assets.models.at("issue_test1"), glm::vec3(20.0f, 2.0f, 20.0f))
        .setScale(glm::vec3(0.01f))
        .play("Unreal Take");
//
//    add<SkeletalInstance>(assets.models.at("issue_test2"), glm::vec3(18.0f, 2.0f, 20.0f))
//            .setScale(glm::vec3(0.01f))
//            .play("Take 001");
//
//    add<SkeletalInstance>(assets.models.at("issue_test3"), glm::vec3(16.0f, 2.0f, 20.0f))
//            .setScale(glm::vec3(0.01f))
//            .play("mixamo.com");
}

void ModelsScene::update(Limitless::Context& context, const Limitless::Camera& camera) {
    Limitless::Scene::update(context, camera);


}