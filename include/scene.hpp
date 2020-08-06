#pragma once

#include <model_instance.hpp>
#include <lighting.hpp>
#include <skybox.hpp>

namespace GraphicsEngine {
    class Scene {
    public:
        Lighting lighting;
    private:
        std::unordered_map<uint64_t, std::unique_ptr<AbstractInstance>> instances;
        std::shared_ptr<Skybox> skybox;

        friend class Render;

        void updateSkeletalInstances() const noexcept;
        void removeDeadInstances() noexcept;
    public:
        template<typename T>
        T& addInstance(T* instance) noexcept {
            instances.emplace(instance->getId(), instance);
            return *instance;
        }
        AbstractInstance& getInstance(uint64_t id) const;

        void setSkybox(const std::string& asset_name);

        void update();
    };
}