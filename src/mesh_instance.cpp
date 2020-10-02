#include <mesh_instance.hpp>

#include <mesh.hpp>

using namespace GraphicsEngine;

MeshInstance::MeshInstance(std::shared_ptr<AbstractMesh> mesh, const std::shared_ptr<Material>& material) noexcept
    : mesh{std::move(mesh)}, material{material} {

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
