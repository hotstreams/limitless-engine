#include <limitless/scene.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/assets.hpp>

using namespace Limitless;

Scene::Scene(Context& context)
    : lighting {context} {
}

AbstractInstance& Scene::operator[](uint64_t id) noexcept { return *instances[id]; }
AbstractInstance& Scene::at(uint64_t id) { return *instances.at(id); }

void Scene::remove(uint64_t id) { instances.erase(id); }

void Scene::clear() {
	instances.clear();
}

void Scene::setSkybox(std::shared_ptr<Skybox> _skybox) {
    skybox = std::move(_skybox);
}

void Scene::update(Context& context, Camera& camera) {
    lighting.update();

    removeDeadInstances();

    for (auto& [id, instance] : instances) {
        instance->update(context, camera);
    }
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

Instances Scene::getWrappers() noexcept {
    Instances wrappers;
    wrappers.reserve(instances.size());

    for (const auto& [id, instance] : instances) {
        wrappers.emplace_back(std::ref(*instance));
    }

    return wrappers;
}

void Scene::clear() {
	instances.clear();
}
