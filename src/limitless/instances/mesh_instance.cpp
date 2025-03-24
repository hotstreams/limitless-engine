#include <limitless/instances/mesh_instance.hpp>

#include <limitless/models/mesh.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/core/context.hpp>

using namespace Limitless;
using namespace Limitless::ms;

MeshInstance::MeshInstance(std::shared_ptr<Mesh> mesh) noexcept
    : mesh {std::move(mesh)}
    , current_lod {0} {
    for (uint32_t i = 0; i < mesh->getLods().size(); ++i) {
        const auto& lod = mesh->getLods()[i];
        lods.emplace(i, LodMaterial(*lod.material));
    }
}

MeshInstance::MeshInstance(const MeshInstance& rhs)
    : mesh {rhs.mesh}
    , current_lod {rhs.current_lod} {
    for (const auto& [index, lod] : rhs.lods) {
        lods.emplace(index, LodMaterial(*lod.material));
    }
}

void MeshInstance::changeBaseMaterial(const std::shared_ptr<ms::Material>& material_) noexcept {
    lods.at(current_lod).base = std::make_shared<Material>(*material_);
    lods.at(current_lod).material = std::make_shared<Material>(*material_);
}

void MeshInstance::changeMaterial(const std::shared_ptr<Material>& material_) noexcept {
    lods.at(current_lod).material = std::make_shared<Material>(*material_);
}

void MeshInstance::reset() noexcept {
    lods.at(current_lod).material = std::make_shared<Material>(*lods.at(current_lod).base);
}

void MeshInstance::selectLod(const Camera& camera, const glm::vec3& position) {
    switch (mesh->getSelection()) {
        case Mesh::LodSelection::CameraDistance:
            {
                const auto distance = glm::distance(camera.getPosition(), position);
                for (uint32_t i = 0; i < mesh->getDistances().size(); ++i) {
                    if (distance <= mesh->getDistances()[i]) {
                        current_lod = i;
                        break;
                    }
                }
            }
            break;
    }
}

void MeshInstance::update(const Camera& camera, const glm::vec3& position) {
    selectLod(camera, position);

    lods.at(current_lod).material->update();
}
