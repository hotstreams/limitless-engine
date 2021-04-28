#pragma once

#include <limitless/lighting/lighting.hpp>
#include <stdexcept>
#include <unordered_map>
#include <memory>

namespace Limitless {
    class AbstractInstance;
    class Camera;
    class Skybox;

    using Instances = std::vector<std::reference_wrapper<AbstractInstance>>;

    class Scene {
    public:
        Lighting lighting;
    private:
        std::unordered_map<uint64_t, std::unique_ptr<AbstractInstance>> instances;
        std::shared_ptr<Skybox> skybox;

        void removeDeadInstances() noexcept;
    public:
        explicit Scene(Context& context);
        virtual ~Scene() = default;

        Scene(const Scene&) = delete;
        Scene(Scene&&) = delete;

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

        void clear();

        AbstractInstance& operator[](uint64_t id) noexcept;
        AbstractInstance& at(uint64_t id);

        auto& getSkybox() noexcept { return skybox; }
        const auto& getSkybox() const noexcept { return skybox; }
        void setSkybox(std::shared_ptr<Skybox> skybox);

        void update(Context& context, Camera& camera);

        auto begin() noexcept { return instances.begin(); }
        auto begin() const noexcept { return instances.begin(); }

        auto end() noexcept { return instances.end(); }
        auto end() const noexcept { return instances.end(); }

        auto& getInstances() noexcept { return instances; }
        Instances getWrappers() noexcept;

        auto size() const noexcept { return instances.size(); }

        void clear();

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