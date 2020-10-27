#include <scene.hpp>
#include <skeletal_instance.hpp>
#include <assets.hpp>

#include <shader_types.hpp>

using namespace GraphicsEngine;

AbstractInstance& Scene::operator[](uint64_t id) noexcept { return *instances[id]; }
AbstractInstance& Scene::at(uint64_t id) { return *instances.at(id); }

void Scene::remove(uint64_t id) { instances.erase(id); }

void Scene::setSkybox(const std::string& asset_name) {
    skybox = assets.skyboxes.at(asset_name);
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

