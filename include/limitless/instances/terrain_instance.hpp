#pragma once

#include <limitless/instances/instanced_instance.hpp>
#include <limitless/ms/material.hpp>

namespace Limitless {
    class TerrainInstance : public InstancedInstance<ModelInstance> {
    private:
        uint32_t tessellation_level {32};
        uint32_t size {1};

        void evaluate();
    public:
        TerrainInstance(const std::shared_ptr<AbstractModel>& model, std::shared_ptr<ms::Material> terrain_material, const glm::vec3& position, uint32_t _size);

        TerrainInstance(const TerrainInstance&) = default;
        TerrainInstance(TerrainInstance&&) noexcept = default;

        TerrainInstance* clone() noexcept override;

        TerrainInstance& setTessLevel(uint32_t _tessellation_level);
    };
}