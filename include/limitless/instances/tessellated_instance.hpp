#pragma once

#include <limitless/instances/instanced_instance.hpp>
#include <limitless/ms/material.hpp>

namespace Limitless {
    class TerrainInstance : public InstancedInstance<ModelInstance> {
    private:
        uint32_t tessellation_level {2048};
        uint32_t size {1024};

        void evaluate() {
            if (tessellation_level > ContextInitializer::limits.max_tess_level) {
                const auto instance_count = std::ceil(tessellation_level / ContextInitializer::limits.max_tess_level);
                auto cp = instances.back()->clone();
                const auto new_size = size / instance_count;
                cp->setScale(glm::vec3{new_size, 1.0f, new_size});
                cp->getMeshes().begin()->second.getMaterial()[0].getTesselationFactor().setValue(glm::vec2(ContextInitializer::limits.max_tess_level));

                instances.clear();
                for (int i = 0; i < instance_count; ++i) {
                    for (int j = 0; j < instance_count; ++j) {
                        auto instance = std::make_unique<ModelInstance>(*cp);
                        instance->setPosition({position.x + new_size * i, position.y, position.z + new_size * j});
                        addInstance(std::move(instance));
                    }
                }
                delete cp;
            }
        }
    public:
        explicit TerrainInstance(const glm::vec3& position)
                : InstancedInstance(position) {
        }

        void setSize(uint32_t _size) {
            size = _size;

        }
        void setTessLevel(uint32_t _tessellation_level) {
            tessellation_level = _tessellation_level;
        }

        void test_eval() {
            evaluate();
        }
    };
}