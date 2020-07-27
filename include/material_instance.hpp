#pragma once

#include <custom_material.hpp>

namespace GraphicsEngine {
    class MaterialInstance {
    private:
        uint64_t next_id {0};
        std::shared_ptr<Material> base;
        std::unordered_map<uint64_t, std::shared_ptr<Material>> materials;
    public:
        explicit MaterialInstance(const std::shared_ptr<Material>& material) {
            base = std::make_shared<Material>(*material);
            materials.emplace(next_id++, base);
        }

        void changeMaterial(const std::shared_ptr<Material>& material) {
            materials.at(0) = std::make_shared<Material>(*material);
        }

        void reset() {
            materials.at(0) = base;
        }

        auto& get(uint64_t id = 0) const { return *materials.at(id); }

        uint64_t apply(const std::shared_ptr<Material>& material) {
            materials.emplace(next_id, std::make_shared<Material>(*material));
            return next_id++;
        }

        void remove(uint64_t id) {
            auto found = materials.find(id);
              if (found == materials.end() || id == 0) {
                  throw std::runtime_error("Attempt to remove base material");
              }

            materials.erase(id);
        }

        std::vector<std::shared_ptr<Material>> getMaterials() const {
            std::vector<std::shared_ptr<Material>> mats;
            for (const auto& [id, mat] : materials) {
                mats.emplace_back(mat);
            }
            return mats;
        }
    };
}