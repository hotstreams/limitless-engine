#include <limitless/scene.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/cpu_profiler.hpp>

using namespace Limitless;

Scene::Scene(Context& context)
    : lighting {context} {
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

void Scene::add(const std::shared_ptr<Instance>& instance) {
    instances.emplace(instance->getId(), instance);
}

void Scene::remove(const std::shared_ptr<Instance>& instance) {
    remove(instance->getId());
}

void Scene::remove(uint64_t id) {
    instances.erase(id);
}

void Scene::removeAll() {
    instances.clear();
}

std::shared_ptr<Instance> Scene::getInstance(uint64_t id) {
    try {
        return instances.at(id);
    } catch (const std::out_of_range& ex) {
        throw scene_exception {"Instance with id = " + std::to_string(id) + " not found!"};
    }
}

std::shared_ptr<ModelInstance> Scene::getModelInstance(uint64_t id) {
    try {
        auto instance = instances.at(id);
        if (instance->getInstanceType() == InstanceType::Model) {
            return std::static_pointer_cast<ModelInstance>(instance);
        } else {
            throw scene_exception {"Instance with id = " + std::to_string(id) + " is not ModelInstance!"};
        }
    } catch (const std::out_of_range& ex) {
        throw scene_exception {"Instance with id = " + std::to_string(id) + " not found!"};
    }
}

std::shared_ptr<SkeletalInstance> Scene::getSkeletalInstance(uint64_t id) {
    try {
            auto instance = instances.at(id);
            if (instance->getInstanceType() == InstanceType::Skeletal) {
                return std::static_pointer_cast<SkeletalInstance>(instance);
            } else {
                throw scene_exception {"Instance with id = " + std::to_string(id) + " is not SkeletalInstance!"};
            }
    } catch (const std::out_of_range& ex) {
        throw scene_exception {"Instance with id = " + std::to_string(id) + " not found!"};
    }
}

std::shared_ptr<EffectInstance> Scene::getEffectInstance(uint64_t id) {
    try {
        auto instance = instances.at(id);
        if (instance->getInstanceType() == InstanceType::Effect) {
            return std::static_pointer_cast<EffectInstance>(instance);
        } else {
            throw scene_exception {"Instance with id = " + std::to_string(id) + " is not EffectInstance!"};
        }
    } catch (const std::out_of_range& ex) {
        throw scene_exception {"Instance with id = " + std::to_string(id) + " not found!"};
    }
}

const Lighting& Scene::getLighting() const noexcept {
    return lighting;
}

Lighting &Scene::getLighting() noexcept {
    return lighting;
}

Light& Scene::add(Light&& light) {
    return lighting.add(std::move(light));
}

Light& Scene::add(const Light &light) {
    return lighting.add(light);
}

const std::shared_ptr<Skybox> &Scene::getSkybox() const noexcept {
    return skybox;
}

std::shared_ptr<Skybox> &Scene::getSkybox() noexcept {
    return skybox;
}

void Scene::setSkybox(const std::shared_ptr<Skybox>& skybox_) {
    skybox = skybox_;
}

void Scene::update(const Camera& camera) {
    CpuProfileScope scope(global_profiler, "Scene::update");

    lighting.update();

    removeDeadInstances();

    for (auto& [_, instance] : instances) {
        CpuProfileScope scope(global_profiler, "Scene::update_instance");
        if (instance->getInstanceType() != InstanceType::Effect) {
            instance->update(camera);
        }
    }

    for (auto& [_, instance] : instances) {
        CpuProfileScope scope(global_profiler, "Scene::update_effect_instance");
        if (instance->getInstanceType() == InstanceType::Effect) {
            instance->update(camera);
        }
    }
}

Instances Scene::getInstances() const noexcept {
    Instances wrappers;
    wrappers.reserve(instances.size());

    const std::function<void(const std::shared_ptr<Instance>&)> visitor = [&] (const std::shared_ptr<Instance>& instance) {
	    wrappers.emplace_back(instance);

	    for (auto& [_, attachment] : instance->getAttachments()) {
	    	visitor(attachment);
	    }
    };

	for (const auto& [_, instance] : instances) {
        if (instance->isHidden()) {
            continue;
        }
		visitor(instance);
	}

    return wrappers;
}
