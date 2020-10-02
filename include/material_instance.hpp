#pragma once

#include <memory>
#include <map>
#include <vector>

namespace GraphicsEngine {
    class Material;

    class MaterialInstance {
    private:
        uint64_t next_id {0};
        std::shared_ptr<Material> base;
        std::map<uint64_t, std::shared_ptr<Material>> materials;
    public:
        explicit MaterialInstance(const std::shared_ptr<Material>& material) noexcept;

        // changes base material
        void changeMaterial(const std::shared_ptr<Material>& material) noexcept;

        // resets base material to first initialized
        void reset() noexcept;

        // applies material as a layer and returns its id
        uint64_t apply(const std::shared_ptr<Material>& material) noexcept;

        // removes 'id' layer
        void remove(uint64_t id);

        // gets material layer; gets base material by default
        [[nodiscard]] auto& get(uint64_t id = 0) const { return *materials.at(id); }

        // gets all material layers
        [[nodiscard]] std::vector<std::shared_ptr<Material>> getMaterials() const noexcept;
    };
}