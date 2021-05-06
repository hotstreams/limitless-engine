#include <limitless/models/model.hpp>

using namespace Limitless;

Model::Model(decltype(meshes)&& _meshes, decltype(materials)&& _materials)
    : AbstractModel(std::move(_meshes)), materials(std::move(_materials)) {
}
