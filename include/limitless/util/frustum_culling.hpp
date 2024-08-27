#pragma once

#include <limitless/instances/model_instance.hpp>

namespace Limitless {
    class FrustumCulling {
    private:
        // contains visible array of instances
        std::vector<std::shared_ptr<Instance>> visible;
        // contains visible array of model instances for each instanced instance
        std::map<uint64_t, std::vector<std::shared_ptr<ModelInstance>>> visible_model_instanced;
    public:
        void update(Scene& scene, Camera& camera) {
            visible.clear();
            visible_model_instanced.clear();

            const auto frustum = Frustum::fromCamera(camera);

            for (auto& instance : scene.getInstances()) {
                if (instance->getInstanceType() == InstanceType::Instanced) {
                    auto& instanced = static_cast<InstancedInstance&>(*instance); //NOLINT

                    for (auto& i: instanced.getInstances()) {
                        if (frustum.intersects(*i)) {
                            visible_model_instanced[instance->getId()].emplace_back(i);
                        }
                    }

                    if (visible_model_instanced.count(instance->getId()) != 0) {
                        visible.emplace_back(instance);
                    }
                } else {
                    if (frustum.intersects(*instance)) {
                        visible.emplace_back(instance);
                    }
                }
            }
        }

        const Instances& getVisibleInstances() const noexcept { return visible; }
        const std::vector<std::shared_ptr<ModelInstance>>& getVisibleModelInstanced(const InstancedInstance& instance) const noexcept { return visible_model_instanced.at(instance.getId()); }
    };
}