#include <mesh_instance.hpp>

using namespace GraphicsEngine;

MeshInstance::MeshInstance(std::shared_ptr<AbstractMesh> mesh, const std::shared_ptr<Material>& material)
    : mesh{std::move(mesh)}, material{material}, hidden{false} {

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
