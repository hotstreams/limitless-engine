#include <limitless/instances/model_instance.hpp>

#include <limitless/models/model.hpp>
#include <limitless/models/elementary_model.hpp>
#include <stdexcept>

using namespace Limitless;

ModelInstance::ModelInstance(ModelShader shader, decltype(model) _model, const glm::vec3& position)
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
    : ModelInstance {ModelShader::Model, _model, _position} {
}

ModelInstance::ModelInstance(decltype(model) _model, std::shared_ptr<ms::Material> material, const glm::vec3& position)
    : AbstractInstance(ModelShader::Model, position)
    , model {std::move(_model)} {
    try {
        auto& elementary_model = dynamic_cast<ElementaryModel&>(*model);

        meshes.emplace(elementary_model.getMesh()->getName(), MeshInstance{elementary_model.getMesh(), material});
    } catch (...) {
        throw std::logic_error{"Wrong model for ModelInstance"};
    }
}

void ModelInstance::draw(Context& ctx, const Assets& assets, ShaderPass pass, ms::Blending blending, const UniformSetter& uniform_setter) {
    if (hidden) {
        return;
    }

    // iterates over all meshes
    for (auto& [name, mesh] : meshes) {
        mesh.draw(ctx, assets, pass, shader_type, final_matrix, blending, uniform_setter);
    }
}

MeshInstance& ModelInstance::operator[](const std::string& mesh) {
    return meshes.at(mesh);
}

ModelInstance* ModelInstance::clone() noexcept {
    return new ModelInstance(*this);
}

void ModelInstance::updateBoundingBox() noexcept {
    bounding_box.center = glm::vec4{position, 1.0f} + glm::vec4{model->getBoundingBox().center, 1.0f} * final_matrix;
    bounding_box.size = glm::vec4{model->getBoundingBox().size, 1.0f} * final_matrix;
}
