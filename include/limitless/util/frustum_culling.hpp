#pragma once

#include <limitless/instances/model_instance.hpp>
#include <limitless/instances/instanced_instance.hpp>
#include <limitless/instances/terrain_instance.hpp>
#include <limitless/core/profiler.hpp>
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
        using VisibleSubinstancesMap = std::map<uint64_t, std::vector<std::shared_ptr<ModelInstance>>>;

        void update(Scene& scene, Camera& camera) {
            CPUProfileScope profile_scope {"FrustumCulling::update"};
            visible.clear();
            visible_instances_of_instanced_instances.clear();

            const auto frustum = Frustum::fromCamera(camera);

            for (auto& instance : scene.getInstances()) {
                if (instance->getInstanceType() == InstanceType::Instanced
                    || instance->getInstanceType() == InstanceType::SkeletalInstanced) {
                    CPUProfileScope profile_scope_instanced {"FC::Instanced"};
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
                    CPUProfileScope profile_scope_terrain {"FC::Terrain"};
                    auto& terrain = static_cast<TerrainInstance&>(*instance); //NOLINT

                    for (auto& i: terrain.getMesh().seams->getInstances()) {
                        if (frustum.intersects(*i)) {
                            visible_instances_of_instanced_instances[terrain.getMesh().seams->getId()].emplace_back(i);
                        }
                    }

                    for (auto& i: terrain.getMesh().trims->getInstances()) {
                        if (frustum.intersects(*i)) {
                            visible_instances_of_instanced_instances[terrain.getMesh().trims->getId()].emplace_back(i);
                        }
                    }

                    for (auto& i: terrain.getMesh().fillers->getInstances()) {
                        if (frustum.intersects(*i)) {
                            visible_instances_of_instanced_instances[terrain.getMesh().fillers->getId()].emplace_back(i);
                        }
                    }

                    for (auto& i: terrain.getMesh().tiles->getInstances()) {
                        if (frustum.intersects(*i)) {
                            visible_instances_of_instanced_instances[terrain.getMesh().tiles->getId()].emplace_back(i);
                        }
                    }

                    if (frustum.intersects(*terrain.getMesh().cross)) {
                        visible_instances_of_instanced_instances[instance->getId()].emplace_back(terrain.getMesh().cross);
                    }

                   if (visible_instances_of_instanced_instances.count(terrain.getMesh().seams->getId()) != 0 ||
                       visible_instances_of_instanced_instances.count(terrain.getMesh().trims->getId()) != 0 ||
                       visible_instances_of_instanced_instances.count(terrain.getMesh().fillers->getId()) != 0 ||
                       visible_instances_of_instanced_instances.count(terrain.getMesh().tiles->getId()) != 0 ||
                       visible_instances_of_instanced_instances.count(instance->getId()) != 0
                   ) {
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
        [[nodiscard]] const std::vector<std::shared_ptr<ModelInstance>>& getVisibleModelInstanced(const InstancedInstance& instance) const noexcept {
            auto it = visible_instances_of_instanced_instances.find(instance.getId());
            static const std::vector<std::shared_ptr<ModelInstance>> empty;
            return it != visible_instances_of_instanced_instances.end() ? it->second : empty;
        }
        [[nodiscard]] const std::vector<std::shared_ptr<ModelInstance>>& getVisibleModelInstanced(uint64_t id) const noexcept {
            auto it = visible_instances_of_instanced_instances.find(id);
            static const std::vector<std::shared_ptr<ModelInstance>> empty;
            return it != visible_instances_of_instanced_instances.end() ? it->second : empty;
        }
        [[nodiscard]] const VisibleSubinstancesMap& getVisibleSubinstancesMap() const noexcept { return visible_instances_of_instanced_instances; }
    };
}