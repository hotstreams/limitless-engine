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
    : scene{ctx} {
        scene.setSkybox(assets.skyboxes.at("skybox"));

        scene.add(Light::builder()
             .color({1.0, 1.0, 1.0, 1.0f})
             .direction({0.5f, -1.0f, 0.0f})
             .build()
        );

        addInstances(assets);
}

void ModelsScene::addInstances(Limitless::Assets& assets) {
    using namespace Limitless;

    auto floor = std::make_shared<InstancedInstance<ModelInstance>>(glm::vec3{0.0f});
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

    scene.add(Instance::builder()
        .model(assets.models.at("thanos"))
        .position(glm::vec3(25.0f, 1.0f, 1.0f))
        .rotation(glm::vec3{-M_PI_2, -M_PI_2, 0.0f})
        .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("daenerys"))
                      .position(glm::vec3(25.0f, 1.0f, 4.0f))
                      .rotation(glm::vec3{-M_PI_2, -M_PI_2, 0.0f})
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("taskmaster"))
                      .position(glm::vec3(25.0f, 1.0f, 7.0f))
                      .rotation(glm::vec3{-M_PI_2, -M_PI_2, 0.0f})
                      .scale(glm::vec3(0.03f))
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("k2"))
                      .position(glm::vec3(25.0f, 1.0f, 10.0f))
                      .rotation(glm::vec3{-M_PI_2, -M_PI_2, 0.0f})
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("skeleton"))
                      .position(glm::vec3(25.0f, 1.0f, 13.0f))
                      .rotation(glm::vec3{-M_PI_2, -M_PI_2, 0.0f})
                      .scale(glm::vec3(0.03f))
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("bob"))
                      .position(glm::vec3(25.0f, 1.0f, 16.0f))
                      .rotation(glm::vec3{0.0f, -M_PI_2, 0.0f})
                      .scale(glm::vec3(0.03f))
                      .build()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("backpack"))
                      .position(glm::vec3(25.0f, 1.5f, 19.0f))
                      .rotation(glm::vec3{0.0f, -M_PI_2, 0.0f})
                      .scale(glm::vec3(0.5f))
                      .asModel()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("cyborg"))
                      .position(glm::vec3(25.0f, 1.0f, 21.0f))
                      .rotation(glm::vec3{0.0f, -M_PI_2, 0.0f})
                      .scale(glm::vec3(0.5f))
                      .asModel()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("drone"))
                      .position(glm::vec3(25.0f, 2.0f, 24.0f))
                      .rotation(glm::vec3{M_PI, M_PI_2, 0.0f})
                      .scale(glm::vec3(0.01f))
                      .asModel()
    );

    scene.add(Instance::builder()
                      .model(assets.models.at("elemental"))
                      .position(glm::vec3(25.0f, 2.0f, 27.0f))
                      .rotation(glm::vec3{-M_PI_2, -M_PI_2, 0.0f})
                      .scale(glm::vec3(10.0f))
                      .asModel()
    );
}

void ModelsScene::update(Limitless::Context& context, const Limitless::Camera& camera) {
   scene.update(context, camera);
}