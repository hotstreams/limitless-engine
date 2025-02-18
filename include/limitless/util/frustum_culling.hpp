#pragma once

#include <limitless/core/profiler.hpp>
#include <limitless/instances/model_instance.hpp>

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

        /**
         * Contains visible MeshInstances of TerrainInstance
         */
        std::map<uint64_t, std::vector<std::reference_wrapper<MeshInstance>>> visible_meshes_of_terrain_instances;
    public:
        void update(Scene& scene, Camera& camera) {
            GPUProfileScope profile_scope {global_gpu_profiler, "Frustum culling"};

            visible.clear();
            visible_instances_of_instanced_instances.clear();
            visible_meshes_of_terrain_instances.clear();

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

                    for (auto& [_, mesh_instance] : terrain.getMeshes()) {
                        if (frustum.intersects(mesh_instance.getMesh()->getBoundingBox())) {
                            visible_meshes_of_terrain_instances[instance->getId()].emplace_back(mesh_instance);
                        }
                    }

                    if (visible_meshes_of_terrain_instances.count(instance->getId()) != 0) {
                        visible.emplace_back(instance);
                    }
                } else {
                    if (frustum.intersects(*instance)) {
                        visible.emplace_back(instance);
                    }
                }
            }
        }

        [[nodiscard]] const Instances& getVisibleInstances() const noexcept { return visible; }
        [[nodiscard]] const std::vector<std::shared_ptr<ModelInstance>>& getVisibleModelInstanced(const InstancedInstance& instance) const noexcept { return visible_instances_of_instanced_instances.at(instance.getId()); }
        const std::vector<std::reference_wrapper<MeshInstance>>& getVisibleTerrainMeshes(const TerrainInstance& instance) const noexcept { return visible_meshes_of_terrain_instances.at(instance.getId()); }
    };
}