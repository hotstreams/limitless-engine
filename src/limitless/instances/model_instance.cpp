#include <limitless/instances/model_instance.hpp>

#include <limitless/models/model.hpp>
#include <limitless/models/elementary_model.hpp>
#include <stdexcept>
#include <utility>

using namespace Limitless;

ModelInstance::ModelInstance(InstanceType shader, decltype(model) _model, const glm::vec3& position)
    : AbstractInstance(shader, position)
    , model {std::move(_model)} {
    try {
        auto& simple_model = dynamic_cast<Model&>(*model);
        auto& model_meshes = simple_model.getMeshes();
        auto& model_mats = simple_model.getMaterials();

        for (uint32_t i = 0; i < model_meshes.size(); ++i) {
            meshes.emplace(model_meshes[i]->getName(), MeshInstance{model_meshes[i], model_mats[i]});
        }
    } catch (...) {
        throw std::logic_error{"Wrong model for ModelInstance"};
    }
}

ModelInstance::ModelInstance(decltype(model) _model, const glm::vec3& _position)
    : ModelInstance {InstanceType::Model, std::move(_model), _position} {
}

ModelInstance::ModelInstance(decltype(model) _model, std::shared_ptr<ms::Material> material, const glm::vec3& position)
    : AbstractInstance(InstanceType::Model, position)
    , model {std::move(_model)} {
    try {
        auto& elementary_model = dynamic_cast<ElementaryModel&>(*model);

        meshes.emplace(elementary_model.getMesh()->getName(), MeshInstance{elementary_model.getMesh(), material});
    } catch (...) {
        throw std::logic_error{"Wrong model for ModelInstance"};
    }
}

void ModelInstance::draw(Context& ctx, const Assets& assets, ShaderType pass, ms::Blending blending, const UniformSetter& uniform_setter) {
    if (hidden) {
        return;
    }

    for (auto& [name, mesh] : meshes) {
        mesh.draw(ctx, assets, pass, shader_type, final_matrix, blending, uniform_setter);
    }
}

MeshInstance& ModelInstance::operator[](const std::string& mesh) {
    try {
        return meshes.at(mesh);
    } catch (...) {
        throw no_such_mesh("with name " + mesh);
    }
}

MeshInstance& ModelInstance::operator[](uint32_t index) {
    if (index >= meshes.size()) {
        throw no_such_mesh("with index " + std::to_string(index));
    }

    return std::next(meshes.begin(), index)->second;
}

std::unique_ptr<AbstractInstance> ModelInstance::clone() noexcept {
    return std::make_unique<ModelInstance>(*this);
}

void ModelInstance::updateBoundingBox() noexcept {
    bounding_box.center = glm::vec4{position, 1.0f} + glm::vec4{model->getBoundingBox().center, 1.0f} * final_matrix;
    bounding_box.size = glm::vec4{model->getBoundingBox().size, 1.0f} * final_matrix;
}

void ModelInstance::update(Context& context, const Camera& camera) {
	AbstractInstance::update(context, camera);

	for (auto& [_, mesh] : meshes) {
		mesh.update();
	}
}
