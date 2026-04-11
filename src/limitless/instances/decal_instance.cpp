#include <limitless/instances/decal_instance.hpp>

#include <limitless/ms/material.hpp>
#include <utility>

using namespace Limitless;

DecalInstance::DecalInstance(std::shared_ptr<Model> model, const std::shared_ptr<ms::Material>& material, const glm::vec3& position)
    : Instance {InstanceType::Decal, position}
    , model {std::move(std::move(model))}
    , material {std::make_shared<ms::Material>(*material)} {
}

void DecalInstance::setMaterial(const std::shared_ptr<ms::Material>& new_material) {
    material = std::make_shared<ms::Material>(*new_material);
}

void DecalInstance::updateBoundingBox() noexcept {
    if (custom_bounding_box) {
        bounding_box = ::Limitless::transformBoundingBox(*custom_bounding_box, final_matrix);
    } else {
        bounding_box = ::Limitless::transformBoundingBox(model->getBoundingBox(), final_matrix);
    }
}

std::unique_ptr<Instance> DecalInstance::clone() noexcept {
    return std::make_unique<DecalInstance>(*this);
}

DecalInstance::DecalInstance(const DecalInstance& rhs)
    : Instance {rhs}
    , model {rhs.model}
    , material {std::make_shared<ms::Material>(*rhs.material)} {
}
