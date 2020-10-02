#include <scene.hpp>
#include <skeletal_instance.hpp>
#include <assets.hpp>

#include <shader_types.hpp>

using namespace GraphicsEngine;

AbstractInstance& Scene::getInstance(uint64_t id) const {
    try {
        return *instances.at(id);
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No such instance.");
    }
}

void Scene::setSkybox(const std::string& asset_name) {
    skybox = assets.skyboxes.get(asset_name);
}

void Scene::update() {
    lighting.update();

    removeDeadInstances();

    updateSkeletalInstances();

    // updates particles
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

void Scene::updateSkeletalInstances() const noexcept {
    for (const auto& [id, instance] : instances) {
        if (instance->getType() == ModelShader::Skeletal) {
            static_cast<SkeletalInstance&>(*instance).update();
        }
    }
}

