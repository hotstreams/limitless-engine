#include "lighting_demoscene.hpp"

#include <limitless/instances/model_instance.hpp>
#include <limitless/instances/instanced_instance.hpp>
#include <random>

using namespace LimitlessDemo;

void LightingScene::addFloor(const Limitless::Assets& assets) {
    using namespace Limitless;

    auto& floor = add<InstancedInstance<ModelInstance>>(glm::vec3{0.0f});
    for (int i = 0; i < FLOOR_INSTANCE_COUNT / 2; ++i) {
        for (int j = 0; j < FLOOR_INSTANCE_COUNT / 2; ++j) {
            floor.addInstance(
                    std::make_unique<ModelInstance>(
                            assets.models.at("plane"),
                            assets.materials.at("floor"),
                            glm::vec3{i, 0.0f, j}
                    )
            );
        }
    }
}

void LightingScene::addSpheres(const Limitless::Assets& assets) {
    using namespace Limitless;
    auto& floor = add<InstancedInstance<ModelInstance>>(glm::vec3{0.0f});
    for (int i = 0; i < FLOOR_INSTANCE_COUNT / 4; ++i) {
        for (int j = 0; j < FLOOR_INSTANCE_COUNT / 4; ++j) {
            floor.addInstance(
                    std::make_unique<ModelInstance>(
                            assets.models.at("sphere"),
                            assets.materials.at("PBR"),
                            glm::vec3{i * 2.0, 0.0f, j * 2.0}
                    )
            );
        }
    }
}

bool LightingScene::isInsideFloor(const glm::vec3& position) {
    if (position.x > FLOOR_INSTANCE_COUNT / 2.0f || position.x < 0.0f) {
        return false;
    }

    if (position.z > FLOOR_INSTANCE_COUNT / 2.0f || position.z < 0.0f) {
        return false;
    }

    return true;
}

void LightingScene::addLights() {
    for (int i = 0; i < LIGHT_COUNT; ++i) {
        lighting.lights.emplace_back(
                glm::vec4{FLOOR_INSTANCE_COUNT / 4.0f, 1.0f, FLOOR_INSTANCE_COUNT / 4.0f, 1.0f},
                glm::vec4{1.0f, 1.0f, 1.0f, 1.0f},
                2.0f
        );
    }
}

LightingScene::LightingScene(Limitless::Context& ctx, const Limitless::Assets& assets)
    : Limitless::Scene(ctx) {
    addFloor(assets);
    addSpheres(assets);
    addLights();

    setSkybox(assets.skyboxes.at("skybox"));

    lighting.ambient_color.a = 0.5f;
    lighting.directional_light = {glm::vec4(1.0, -1.0, 0.5, 1.0f), glm::vec4{1.0f}};
}

void LightingScene::update(Limitless::Context& context, const Limitless::Camera& camera) {
    Limitless::Scene::update(context, camera);

    using namespace std::chrono;
    auto current_time = steady_clock::now();
    static auto last_time = steady_clock::now();
    auto delta_time = duration_cast<duration<float>>(current_time - last_time).count();
    last_time = current_time;

    std::default_random_engine e {std::random_device{}()};
    std::uniform_real_distribution<> dis_direction(-1.0, 1.0);
    std::uniform_real_distribution<> dis_duration(1.0, 2.0);
    std::uniform_real_distribution<> dis_color(0.0, 1.0);

    for (std::size_t i = 0; i < data.size(); ++i) {
        auto& light = lighting.lights.at(i);
        auto& light_data = data[i];

        light.position += glm::vec4(light_data.direction, 1.0) * delta_time * 10.0f;
        light.position = glm::clamp(light.position, glm::vec4(0.0f), glm::vec4(FLOOR_INSTANCE_COUNT / 2.0f));

        if (light_data.duration <= 0.0f) {
            light_data.direction = glm::vec3(dis_direction(e), 0.0f, dis_direction(e));
            light_data.duration = dis_duration(e);
            light.color = glm::vec4(dis_color(e), dis_color(e), dis_color(e), 1.0f);
        } else {
            light_data.duration -= delta_time;
        }

        if (!isInsideFloor(light.position)) {
            light_data.direction *= -0.3f;
        }
    }
}
