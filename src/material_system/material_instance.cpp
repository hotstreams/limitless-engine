#include <material_system/material_instance.hpp>

#include <material_system/material.hpp>
#include <stdexcept>

using namespace GraphicsEngine;

MaterialInstance::MaterialInstance(const std::shared_ptr<Material>& material) noexcept {
    base = std::make_shared<Material>(*material);
    materials.emplace(next_id++, base);
}

void MaterialInstance::changeMaterial(const std::shared_ptr<Material>& material) noexcept {
    materials[0] = std::make_shared<Material>(*material);
}

void MaterialInstance::reset() noexcept {
    materials[0] = base;
}

void MaterialInstance::clear() noexcept {
    materials.erase(++materials.begin(), materials.end());
}

uint64_t MaterialInstance::apply(const std::shared_ptr<Material>& material) noexcept {
    materials.emplace(next_id, std::make_shared<Material>(*material));
    return next_id++;
}

void MaterialInstance::remove(uint64_t id) {
    if (id == 0) {
        throw std::runtime_error("Attempt to remove base material layer.");
    }

    materials.erase(id);
}
