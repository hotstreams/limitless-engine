#include <limitless/scene.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/assets.hpp>

#include <limitless/shader_types.hpp>

using namespace LimitlessEngine;

AbstractInstance& Scene::operator[](uint64_t id) noexcept { return *instances[id]; }
AbstractInstance& Scene::at(uint64_t id) { return *instances.at(id); }

void Scene::remove(uint64_t id) { instances.erase(id); }

void Scene::setSkybox(std::shared_ptr<Skybox> _skybox) {
    skybox = std::move(_skybox);
}

void Scene::update() {
    lighting.update();

    removeDeadInstances();

    for (auto& [id, instance] : instances)
        instance->update();
}

void Scene::removeDeadInstances() noexcept {
    for (auto it = instances.cbegin(); it != instances.cend(); ) {
        if (it->second->isKilled()) {
            it = instances.erase(it);
        } else {
            ++it;
        }
    }
}

