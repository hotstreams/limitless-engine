#include <limitless/instances/model_instance.hpp>

#include <limitless/models/model.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <stdexcept>
#include <utility>
#include <algorithm>

using namespace Limitless;

ModelInstance::ModelInstance(InstanceType shader, decltype(model) _model, const glm::vec3& position)
    : Instance(shader, position)
    , model {std::move(_model)}
    , lod_group {model} {
    // Ensure local AABB is always available for GPU-side effects (wind, etc.).
    // This populates Instance::custom_bounding_box which we also pack into the instance buffer.
    if (!custom_bounding_box) {
        setBoundingBox(model->getBoundingBox());
    }
    updateBoundingBox();
}

ModelInstance::ModelInstance(decltype(model) _model, const glm::vec3& _position)
    : ModelInstance {InstanceType::Model, std::move(_model), _position} {
}

MeshInstance& ModelInstance::operator[](const std::string& mesh) {
    try {
        return lod_group.getCurrentMeshes().at(mesh);
    } catch (...) {
        throw no_such_mesh("with name " + mesh);
    }
}

MeshInstance& ModelInstance::operator[](uint32_t index) {
    if (index >= lod_group.getCurrentMeshes().size()) {
        throw no_such_mesh("with index " + std::to_string(index));
    }

    return std::next(lod_group.getCurrentMeshes().begin(), index)->second;
}

std::unique_ptr<Instance> ModelInstance::clone() noexcept {
    auto instance = std::make_unique<ModelInstance>(*this);

    return instance;
}

void ModelInstance::updateBoundingBox() noexcept {
    if (custom_bounding_box) {
        bounding_box = ::Limitless::transformBoundingBox(*custom_bounding_box, final_matrix);
    } else {
        bounding_box = ::Limitless::transformBoundingBox(model->getBoundingBox(), final_matrix);
    }
}

void ModelInstance::update(const Camera &camera) {
    Instance::update(camera);

    lod_group.update(camera, position);

    updateBoundingBox();
}

void ModelInstance::changeMaterial(uint32_t mesh_index, const std::shared_ptr<ms::Material> &material) {
    if (mesh_index >= lod_group.getCurrentMeshes().size()) {
        throw no_such_mesh("with index " + std::to_string(mesh_index));
    }

    std::next(lod_group.getCurrentMeshes().begin(), mesh_index)->second.changeMaterial(material);
}

void ModelInstance::changeMaterial(const std::string& mesh_name, const std::shared_ptr<ms::Material> &material) {
    try {
        lod_group.getCurrentMeshes().at(mesh_name).changeMaterial(material);
    } catch (...) {
        throw no_such_mesh("with name " + mesh_name);
    }
}

void ModelInstance::changeMaterials(const std::shared_ptr<ms::Material> &material) {
    for (auto& [_, mesh] : lod_group.getCurrentMeshes()) {
        mesh.changeMaterial(material);
    }
}

void ModelInstance::changeBaseMaterial(uint32_t mesh_index, const std::shared_ptr<ms::Material> &material) {
    if (mesh_index >= lod_group.getCurrentMeshes().size()) {
        throw no_such_mesh("with index " + std::to_string(mesh_index));
    }

    std::next(lod_group.getCurrentMeshes().begin(), mesh_index)->second.changeBaseMaterial(material);
}

void ModelInstance::changeBaseMaterial(const std::string& mesh_name, const std::shared_ptr<ms::Material> &material) {
    try {
        lod_group.getCurrentMeshes().at(mesh_name).changeBaseMaterial(material);
    } catch (...) {
        throw no_such_mesh("with name " + mesh_name);
    }
}

void ModelInstance::changeBaseMaterials(const std::shared_ptr<ms::Material>& material) {
    for (auto& [_, mesh]: lod_group.getCurrentMeshes()) {
        mesh.changeBaseMaterial(material);
    }
}

void ModelInstance::resetMaterial(uint32_t mesh_index) {
    if (mesh_index >= lod_group.getCurrentMeshes().size()) {
        throw no_such_mesh("with index " + std::to_string(mesh_index));
    }

    std::next(lod_group.getCurrentMeshes().begin(), mesh_index)->second.reset();
}

void ModelInstance::resetMaterial(const std::string& mesh_name) {
    try {
        lod_group.getCurrentMeshes().at(mesh_name).reset();
    } catch (...) {
        throw no_such_mesh("with name " + mesh_name);
    }
}

void ModelInstance::resetMaterials() {
    for (auto& [_, mesh] : lod_group.getCurrentMeshes()) {
        mesh.reset();
    }
}

MeshInstance& ModelInstance::getMeshInstance(const std::string &mesh) {
    try {
        return lod_group.getCurrentMeshes().at(mesh);
    } catch (...) {
        throw no_such_mesh("with name " + mesh);
    }
}

MeshInstance& ModelInstance::getMeshInstance(uint32_t index) {
    if (index >= lod_group.getCurrentMeshes().size()) {
        throw no_such_mesh("with index " + std::to_string(index));
    }

    return std::next(lod_group.getCurrentMeshes().begin(), index)->second;
}

const std::shared_ptr<ms::Material> &ModelInstance::getMaterial(uint32_t mesh_index) {
    if (mesh_index >= lod_group.getCurrentMeshes().size()) {
        throw no_such_mesh("with index " + std::to_string(mesh_index));
    }

    return std::next(lod_group.getCurrentMeshes().begin(), mesh_index)->second.getMaterial();
}

const std::shared_ptr<ms::Material> &ModelInstance::getMaterial(const std::string& mesh_name) {
    try {
        return lod_group.getCurrentMeshes().at(mesh_name).getMaterial();
    } catch (...) {
        throw no_such_mesh("with name " + mesh_name);
    }
}

void ModelInstance::setMaterialVariant(size_t variant_index) {
    lod_group.applyMaterialVariantFromModel(*model, variant_index);
}

void ModelInstance::setMaterialVariant(const std::string& variant_name) {
    const auto* vs = model->getMaterialVariantSet();
    if (!vs) {
        throw std::runtime_error("ModelInstance::setMaterialVariant: model has no material variant set");
    }
    const auto& names = vs->variant_names;
    const auto it = std::find(names.begin(), names.end(), variant_name);
    if (it == names.end()) {
        throw std::out_of_range("unknown material variant name: " + variant_name);
    }
    setMaterialVariant(static_cast<size_t>(it - names.begin()));
}
