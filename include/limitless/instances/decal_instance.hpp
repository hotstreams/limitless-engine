#pragma once

#include <limitless/instances/instance.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/models/elementary_model.hpp>

namespace Limitless {
    class DecalInstance : public Instance {
    private:
        std::shared_ptr<AbstractModel> model;
        std::shared_ptr<ms::Material> material;
        uint8_t projection_mask {0xFF};
    public:
        DecalInstance(std::shared_ptr<AbstractModel> model, const std::shared_ptr<ms::Material>& material, const glm::vec3& position);

        DecalInstance(const DecalInstance&);

        void updateBoundingBox() noexcept override;
        std::unique_ptr<Instance> clone() noexcept override;

        void setMaterial(const std::shared_ptr<ms::Material>& new_material);

        auto& getMaterial() noexcept { return material; }
        auto& getModel() noexcept { return model; }
        auto& getProjectionMask() noexcept { return projection_mask; }
    };
}