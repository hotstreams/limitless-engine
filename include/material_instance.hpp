#pragma once

#include <material.hpp>

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
            materials[0] = std::make_shared<Material>(*material);
        }

        void reset() {
            materials[0] = base;
        }

        auto& get() const { return *materials.at(0); }
        auto& get(uint64_t id) const { return *materials.at(id); }

//        uint64_t applyMaterial(const Material& material) {
//
//        }
//
//        void removeMaterial(uint64_t id) {
//              if (id == 0)
//                    throw std::runtime("Attempt to remove base material");
//        }
    };
}