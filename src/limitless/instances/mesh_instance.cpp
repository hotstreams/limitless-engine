#include <limitless/instances/mesh_instance.hpp>

#include <limitless/models/mesh.hpp>

using namespace LimitlessEngine;

MeshInstance::MeshInstance(std::shared_ptr<AbstractMesh> _mesh, const std::shared_ptr<Material>& _material) noexcept
    : mesh{std::move(_mesh)}, material{_material} {

}

void MeshInstance::hide() noexcept {
    hidden = true;
}

void MeshInstance::reveal() noexcept {
    hidden = false;
}

void MeshInstance::draw() const noexcept {
    mesh->draw();
}
