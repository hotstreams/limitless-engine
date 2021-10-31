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

void Scene::setSkybox(std::shared_ptr<Skybox> _skybox) {
    skybox = std::move(_skybox);
}

void Scene::update(Context& context, const Camera& camera) {
    lighting.update();

    removeDeadInstances();

    for (auto& [_, instance] : instances) {
        if (instance->getShaderType() != ModelShader::Effect) {
            instance->update(context, camera);
        }
    }

    for (auto& [_, instance] : instances) {
        if (instance->getShaderType() == ModelShader::Effect) {
            instance->update(context, camera);
        }
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

    const std::function<void(AbstractInstance&)> visitor = [&] (AbstractInstance& instance) {
	    wrappers.emplace_back(std::ref(instance));

	    for (auto& [_, attachment] : instance.getAttachments()) {
	    	visitor(*attachment);
	    }
    };

	for (const auto& [_, instance] : instances) {
		visitor(*instance);
	}

    return wrappers;
}

void Scene::clear() {
	instances.clear();
}
