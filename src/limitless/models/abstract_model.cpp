#include <limitless/models/abstract_model.hpp>

#include <limitless/models/mesh.hpp>

using namespace LimitlessEngine;

AbstractModel::AbstractModel(decltype(meshes)&& _meshes)
    : meshes { std::move(_meshes) } {
    calculateBoundingBox();
}

void AbstractModel::calculateBoundingBox() {
    if (!meshes.empty()) {
        bounding_box = meshes[0]->getBoundingBox();
        for (size_t i = 1; i < meshes.size(); ++i) {
            bounding_box = mergeBoundingBox(bounding_box, meshes[i]->getBoundingBox());
        }
    }
}
