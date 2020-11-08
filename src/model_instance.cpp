#include <model_instance.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <model.hpp>
#include <shader_storage.hpp>

using namespace GraphicsEngine;

ModelInstance::ModelInstance(decltype(model) _model, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale)
    : ModelInstance{nullptr, std::move(_model), _position, _rotation, _scale} {
}

ModelInstance::ModelInstance(Lighting* lighting, decltype(model) _model, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale)
    : AbstractInstance{lighting, ModelShader::Model, _position, _rotation, _scale}, model{std::move(_model)}, model_matrix{1.0f} {
    auto& simple_model = dynamic_cast<Model&>(*model);

    auto& model_meshes = simple_model.getMeshes();
    auto& model_mats = simple_model.getMaterials();

    for(uint32_t i = 0; i < model_meshes.size(); ++i) {
        meshes.emplace(model_meshes[i]->getName(), MeshInstance{model_meshes[i], model_mats[i]});
    }
}

void ModelInstance::calculateModelMatrix() noexcept {
    model_matrix = glm::mat4{1.0f};

    model_matrix = glm::translate(model_matrix, position);

    model_matrix = glm::rotate(model_matrix, rotation.x, glm::vec3{1.0f, 0.f, 0.f});
    model_matrix = glm::rotate(model_matrix, rotation.y, glm::vec3{0.0f, 1.f, 0.f});
    model_matrix = glm::rotate(model_matrix, rotation.z, glm::vec3{0.0f, 0.f, 1.f});

    model_matrix = glm::scale(model_matrix, scale);
}

void ModelInstance::draw(MaterialShader material_type, Blending blending, const UniformSetter& uniform_setter) {
    if (hidden) {
        return;
    }

    calculateModelMatrix();

    // iterates over all visible meshes
    for (auto& [name, mesh] : meshes) {
        if (mesh.isHidden()) {
            continue;
        }

        // iterates over all material layers
        auto first_opaque {true};
        for (const auto& [layer, material] : mesh.getMaterial()) {
            // following blending order
            if (material->getBlending() == blending) {
                // sets blending mode dependent on layers count
                if (blending == Blending::Opaque && mesh.getMaterial().count() > 1 && !first_opaque) {
                    setBlendingMode(Blending::OpaqueHalf);
                } else {
                    setBlendingMode(blending);
                    if (blending == Blending::Opaque) first_opaque = false;
                }

                // gets required shader from storage
                auto& shader = *shader_storage.get(material_type, shader_type, material->getShaderIndex());

                // updates model/material uniforms
                shader << *material
                       << UniformValue{"model", model_matrix};

                // sets custom pass-dependent uniforms
                for (const auto& uniform_set : uniform_setter) {
                    uniform_set(shader);
                }

                shader.use();

                mesh.draw();
            }
        }
    }
}

MeshInstance& ModelInstance::operator[](const std::string& mesh) {
    return meshes.at(mesh);
}

ModelInstance* ModelInstance::clone() noexcept {
    return new ModelInstance(*this);
}
