#pragma once

#include <model_instance.hpp>
#include <lighting.hpp>

namespace GraphicsEngine {
    class Scene {
    public:
        Lighting lighting;
    private:
        std::unordered_map<uint64_t, std::unique_ptr<AbstractInstance>> instances;

        friend class Render;
    public:
        template<typename T>
        T& addInstance(T* instance) noexcept {
            instances.emplace(instance->getId(), instance);
            return *instance;
        }

        auto& getInstance(uint64_t id) const {
            return *instances.at(id);
        }

        void update() {
            // checks instances for killed ones
            for (auto it = instances.cbegin(); it != instances.cend(); ) {
                if (it->second->isKilled()) {
                    it = instances.erase(it);
                } else {
                    ++it;
                }
            }

            // updates lighting
            lighting.update();

            // updates skeletal models


            // updates particles
        }
    };
}