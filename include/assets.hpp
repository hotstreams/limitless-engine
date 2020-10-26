#pragma once

#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <iterator>

#define ASSETS_DIR "../assets/"

namespace GraphicsEngine {
    class AbstractMesh;
    class AbstractModel;
    class EffectInstance;
    class Texture;
    class Material;
    class Skybox;

    struct resource_container_error : public std::runtime_error {
        explicit resource_container_error(const std::string& error) : runtime_error(error) {}
    };

    template<typename T>
    class ResourceContainer {
    private:
        std::unordered_map<std::string, std::shared_ptr<T>> resource;
    public:
        const auto& operator[](const std::string& name) noexcept {
            return resource[name];
        }

        const auto& at(const std::string& name) const {
            try {
                return resource.at(name);
            } catch (...) {
                throw resource_container_error("No such resource called " + name);
            }
        }

        void add(const std::string& name, std::shared_ptr<T> res) {
            const auto result = resource.emplace(name, std::move(res));
            if (!result.second) {
                throw resource_container_error("Failed to add resource " + name + ", already exists.");
            }
        }

        void remove(const std::string& name) {
            resource.erase(name);
        }

        [[nodiscard]] bool isExist(const std::string& name) const noexcept {
            return resource.find(name) != resource.end();
        }

        auto begin() noexcept { return resource.begin(); }
        auto begin() const noexcept { return resource.begin(); }

        auto end() noexcept { return resource.end(); }
        auto end() const noexcept { return resource.end(); }
    };

    struct Assets {
        ResourceContainer<AbstractModel> models;
        ResourceContainer<AbstractMesh> meshes;
        ResourceContainer<Texture> textures;
        ResourceContainer<Material> materials;
        ResourceContainer<Skybox> skyboxes;
        ResourceContainer<EffectInstance> effects;

        void load();
    };

    inline Assets assets;
}