#include <limitless/instances/mesh_instance.hpp>

#include <limitless/models/mesh.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/core/context.hpp>

using namespace Limitless;
using namespace Limitless::ms;

MeshInstance::MeshInstance(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<ms::Material>& material) noexcept
    : mesh {mesh}
    , material {std::make_shared<Material>(*material)}
    , base {std::make_shared<Material>(*material)} {
}

MeshInstance::MeshInstance(const MeshInstance& rhs)
    : mesh {rhs.mesh} 
    , material {std::make_shared<Material>(*rhs.material)}
    , base {std::make_shared<Material>(*rhs.base)} {
}

void MeshInstance::changeBaseMaterial(const std::shared_ptr<ms::Material>& material_) noexcept {
    base = std::make_shared<Material>(*material_);
}

void MeshInstance::changeMaterial(const std::shared_ptr<Material>& material_) noexcept {
    material = std::make_shared<Material>(*material_);
}

void MeshInstance::reset() noexcept {
    material = std::make_shared<Material>(*base);
}

void MeshInstance::update() {
    material->update();
}
