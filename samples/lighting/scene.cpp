#include "scene.hpp"

#include <limitless/scene.hpp>
#include <limitless/instances/instanced_instance.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/lighting/light.hpp>
#include <random>

using namespace LimitlessMaterials;
using namespace Limitless;

LimitlessMaterials::Scene::Scene(Limitless::Context& ctx, Limitless::Assets& assets)
    : scene(ctx)
    , assets {assets} {
    setUpLighting();
    setUpModels();
}

bool LimitlessMaterials::Scene::isInsideFloor(const glm::vec3& position) {
    if (position.x > FLOOR_INSTANCE_COUNT / 2.0f || position.x < 0.0f) {
        return false;
    }

    if (position.z > FLOOR_INSTANCE_COUNT / 2.0f || position.z < 0.0f) {
        return false;
    }

    return true;
}

void LimitlessMaterials::Scene::update(Limitless::Context& context, const Limitless::Camera& camera) {
    scene.update(context, camera);

    using namespace std::chrono;
    auto current_time = steady_clock::now();
    static auto last_time = steady_clock::now();
    auto delta_time = duration_cast<duration<float>>(current_time - last_time).count();
    last_time = current_time;

    std::default_random_engine e {std::random_device{}()};
    std::uniform_real_distribution<> dis_direction(-1.0, 1.0);
    std::uniform_real_distribution<> dis_duration(1.0, 2.0);
    std::uniform_real_distribution<> dis_color(0.0, 1.0);

    uint32_t i = 0;
    for (auto& [_, light]: scene.getLighting().getLights()) {
        auto& light_data = data[i++];

        light.getPosition() += glm::vec3(light_data.direction) * delta_time * 10.0f;
        light.getPosition() = glm::clamp(light.getPosition(), glm::vec3(0.0f), glm::vec3(64 / 2.0f));

        if (light_data.duration <= 0.0f) {
            light_data.direction = glm::vec3(dis_direction(e), 0.0f, dis_direction(e));
            light_data.duration = dis_duration(e);
            light.setColor(glm::vec4(dis_color(e), dis_color(e), dis_color(e), 1.0f));
        } else {
            light_data.duration -= delta_time;
        }

        if (!isInsideFloor(light.getPosition())) {
            light_data.direction *= -0.3f;
        }
    }
}

void LimitlessMaterials::Scene::setUpModels() {
    using namespace Limitless;

    auto floor = std::make_shared<InstancedInstance>();
    for (int i = 0; i < FLOOR_INSTANCE_COUNT / 2; ++i) {
        for (int j = 0; j < FLOOR_INSTANCE_COUNT / 2; ++j) {
            floor->add(Instance::builder()
                .model(assets.models.at("plane"))
                .material(assets.materials.at("basic1"))
                .position(glm::vec3{i, 0.0f, j})
                .asModel()
            );
        }
    }
    scene.add(floor);

    using namespace Limitless;
    auto spheres = std::make_shared<InstancedInstance>();
    for (int i = 0; i < FLOOR_INSTANCE_COUNT / 4; ++i) {
        for (int j = 0; j < FLOOR_INSTANCE_COUNT / 4; ++j) {
            spheres->add(Instance::builder()
                .model(assets.models.at("sphere"))
                .material(assets.materials.at("PBR"))
                .position(glm::vec3{i * 2.0, 1.0f, j * 2.0})
                .asModel()
            );
        }
    }
    scene.add(spheres);
}

void LimitlessMaterials::Scene::setUpLighting() {
    scene.getLighting().getAmbientColor().a = 0.7f;
    scene.add(Light::builder()
      .color(glm::vec4(1.0, 1.0, 0.5, 1.0f))
      .direction(glm::vec3{-1.0f})
      .build()
    );

    for (int i = 0; i < LIGHT_COUNT; ++i) {
        scene.add(Light::builder()
              .position({FLOOR_INSTANCE_COUNT / 4.0f, 1.0f, FLOOR_INSTANCE_COUNT / 4.0f})
              .color({1.0f, 1.0f, 1.0f, 1.0f})
              .radius(2.0f)
              .build()
        );
    }
}
