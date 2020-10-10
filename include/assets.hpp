#pragma once

#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <iterator>

#define ASSETS_DIR "../assets/"

namespace GraphicsEngine {
    class AbstractMesh;
    class AbstractModel;
    class Texture;
    class Material;
    class Skybox;

    template<typename T>
    class ResourceContainer {
    private:
        std::unordered_map<std::string, std::shared_ptr<T>> resource;
    public:
        [[nodiscard]] const auto& get(const std::string& name) const {
            try {
                return resource.at(name);
            } catch (const std::out_of_range& e) {
                throw std::runtime_error("No such resource called " + name);
            }
        }

        void add(const std::string& name, std::shared_ptr<T> res) {
            const auto result = resource.emplace(name, std::move(res));
            if (!result.second) {
                throw std::runtime_error("Failed to add resource " + name + ", already exists.");
            }
        }

        [[nodiscard]] bool isExist(const std::string& name) const noexcept {
            return resource.find(name) != resource.end();
        }
    };

    struct Assets {
        ResourceContainer<AbstractModel> models;
        ResourceContainer<AbstractMesh> meshes;
        ResourceContainer<Texture> textures;
        ResourceContainer<Material> materials;
        ResourceContainer<Skybox> skyboxes;

        void load();
    };

    inline Assets assets;
}