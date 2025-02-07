#include <limitless/models/model.hpp>
#include <limitless/models/model_builder.h>

using namespace Limitless;

Model::Model(decltype(meshes)&& _meshes, decltype(materials)&& _materials, std::string name)
    : AbstractModel(std::move(_meshes), std::move(name))
    , materials(std::move(_materials)) {
}

Model::Builder Model::builder() {
    return {};
}

