#include <iostream>
#include <limitless/instances/lod_group_instance.hpp>

using namespace Limitless;

LodGroupInstance::LodGroupInstance(const std::shared_ptr<Model>& model)
    : current_lod {0}
    , lod_selection {model->getSelection()}
    , lod_transition {model->getTransition()}
    , distances {model->getDistances()}
{
    for (const auto& [meshes, materials] : model->getLods()) {
        Lod lod;

        for (uint32_t i = 0; i < meshes.size(); ++i) {
            const auto& mesh = meshes.at(i);
            const auto& material = materials.at(i);
            lod.emplace(mesh->getName(), MeshInstance {mesh, std::make_shared<ms::Material>(*material)});
        }

        lods.emplace_back(std::move(lod));
    }
}

void LodGroupInstance::selectLod(const Camera& camera, const glm::vec3& position) {
    switch (lod_selection) {
        case LodSelection::CameraDistance:
            {
                const auto distance = glm::distance(camera.getPosition(), position);
                for (uint32_t i = 0; i < distances.size(); ++i) {
                    if (distance <= distances[i]) {
                        current_lod = i;
                        break;
                    }
                }
            }
            break;
    }
}

void LodGroupInstance::update(const Camera& camera, const glm::vec3& position) {
    selectLod(camera, position);

    for (auto& [_, mesh] : lods[current_lod]) {
        mesh.update();
    }
}