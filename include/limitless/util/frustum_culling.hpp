#pragma once

#include <limitless/instances/model_instance.hpp>
#include <iostream>

namespace Limitless {
    class FrustumCulling {
    private:
        /**
         * Contains visible array of simple instances
         */
        std::vector<std::shared_ptr<Instance>> visible;

        /**
         * Contains visible array of model instances for each instanced instance
         */
        std::map<uint64_t, std::vector<std::shared_ptr<ModelInstance>>> visible_instances_of_instanced_instances;
    public:
        void update(Scene& scene, Camera& camera) {
            visible.clear();
            visible_instances_of_instanced_instances.clear();

            const auto frustum = Frustum::fromCamera(camera);

            for (auto& instance : scene.getInstances()) {
                if (instance->getInstanceType() == InstanceType::Instanced) {
                    auto& instanced = static_cast<InstancedInstance&>(*instance); //NOLINT

                    for (auto& i: instanced.getInstances()) {
                        if (frustum.intersects(*i)) {
                            visible_instances_of_instanced_instances[instance->getId()].emplace_back(i);
                        }
                    }

                    if (visible_instances_of_instanced_instances.count(instance->getId()) != 0) {
                        visible.emplace_back(instance);
                    }
                } else if (instance->getInstanceType() == InstanceType::Terrain) {
                    auto& terrain = static_cast<TerrainInstance&>(*instance); //NOLINT

                    for (auto& i: terrain.mesh.seams->getInstances()) {
                        if (frustum.intersects(*i)) {
                            visible_instances_of_instanced_instances[terrain.mesh.seams->getId()].emplace_back(i);
                        }
                    }

                    for (auto& i: terrain.mesh.trims->getInstances()) {
                        if (frustum.intersects(*i)) {
                            visible_instances_of_instanced_instances[terrain.mesh.trims->getId()].emplace_back(i);
                        }
                    }

                    for (auto& i: terrain.mesh.fillers->getInstances()) {
                        if (frustum.intersects(*i)) {
                            visible_instances_of_instanced_instances[terrain.mesh.fillers->getId()].emplace_back(i);
                        }
                    }

                    for (auto& i: terrain.mesh.tiles->getInstances()) {
                        if (frustum.intersects(*i)) {
                            visible_instances_of_instanced_instances[terrain.mesh.tiles->getId()].emplace_back(i);
                        }
                    }

                    if (frustum.intersects(*terrain.mesh.cross)) {
                        visible_instances_of_instanced_instances[instance->getId()].emplace_back(terrain.mesh.cross);
                    }

//                    if (
//                            visible_instances_of_instanced_instances.count(terrain.mesh.seams->getId()) != 0 ||
//                        visible_instances_of_instanced_instances.count(terrain.mesh.trims->getId()) != 0 ||
//                        visible_instances_of_instanced_instances.count(terrain.mesh.fillers->getId()) != 0 ||
//                        visible_instances_of_instanced_instances.count(terrain.mesh.tiles->getId()) != 0 ||
//                        visible_instances_of_instanced_instances.count(instance->getId()) != 0
//                    ) {
                        visible.emplace_back(instance);
//                    }
                } else {
                    if (frustum.intersects(*instance)) {
                        visible.emplace_back(instance);
                    }
                }
            }
        }

        [[nodiscard]] const Instances& getVisibleInstances() const noexcept { return visible; }
        [[nodiscard]] const std::vector<std::shared_ptr<ModelInstance>>& getVisibleModelInstanced(const InstancedInstance& instance) noexcept { return visible_instances_of_instanced_instances[instance.getId()]; }
        [[nodiscard]] const std::vector<std::shared_ptr<ModelInstance>>& getVisibleModelInstanced(uint64_t id) noexcept { return visible_instances_of_instanced_instances[id]; }
    };
}