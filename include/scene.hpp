#pragma once

#include <lighting.hpp>
#include <skybox.hpp>
#include <effect_renderer.hpp>

namespace GraphicsEngine {
    class AbstractInstance;

    class Scene {
    public:
        Lighting lighting;
    private:
        std::unordered_map<uint64_t, std::unique_ptr<AbstractInstance>> instances;
        std::shared_ptr<Skybox> skybox;

        void removeDeadInstances() noexcept;
    public:
        template<typename T>
        T& add(T* instance) noexcept {
            static_assert(std::is_base_of_v<AbstractInstance, T>, "Typename type must be base of AbstractInstance");

            instances.emplace(instance->getId(), instance);
            return *instance;
        }

        template<typename T, typename... Args>
        T& add(Args&&... args) {
            static_assert(std::is_base_of_v<AbstractInstance, T>, "Typename type must be base of AbstractInstance");

            T* instance = new T(&lighting, std::forward<Args>(args)...);
            instances.emplace(instance->getId(), instance);
            return *instance;
        }

        void remove(uint64_t id);

        AbstractInstance& operator[](uint64_t id) noexcept;
        AbstractInstance& at(uint64_t id);

        auto& getSkybox() noexcept { return skybox; }
        auto& getSkybox() const noexcept { return skybox; }
        void setSkybox(const std::string& asset_name);

        virtual void update();

        auto begin() noexcept { return instances.begin(); }
        auto begin() const noexcept { return instances.begin(); }

        auto end() noexcept { return instances.end(); }
        auto end() const noexcept { return instances.end(); }

        auto size() const noexcept { return instances.size(); }

#ifdef NDEBUG
        template<typename T>
        T& get(uint64_t id) {
            try {
                return dynamic_cast<T&>(*instances[id]);
            } catch (...) {
                throw std::runtime_error("Scene wrong instance cast");
            }
        }
#else
        template<typename T>
        T& get(uint64_t id) noexcept {
            return static_cast<T&>(*instances[id]);
        }
#endif
    };
}