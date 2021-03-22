#include <limitless/instances/model_instance.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <limitless/models/model.hpp>
#include <limitless/shader_storage.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/assets.hpp>

using namespace LimitlessEngine;

ModelInstance::ModelInstance(decltype(model) _model, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale)
    : ModelInstance{nullptr, std::move(_model), _position, _rotation, _scale} {
}

ModelInstance::ModelInstance(Lighting* lighting, decltype(model) _model, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale)
    : AbstractInstance{lighting, ModelShader::Model, _position, _rotation, _scale}, model{std::move(_model)} {
    auto& simple_model = dynamic_cast<Model&>(*model);

    auto& model_meshes = simple_model.getMeshes();
    auto& model_mats = simple_model.getMaterials();

    for(uint32_t i = 0; i < model_meshes.size(); ++i) {
        meshes.emplace(model_meshes[i]->getName(), MeshInstance{model_meshes[i], model_mats[i]});
    }
}

void ModelInstance::draw(const Assets& assets, MaterialShader material_type, Blending blending, const UniformSetter& uniform_setter) {
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
                auto& shader = assets.shaders.get(material_type, shader_type, material->getShaderIndex());

                // updates model/material uniforms
                shader << *material
                       << UniformValue{"model", model_matrix};

                // sets custom pass-dependent uniforms
                std::for_each(uniform_setter.begin(), uniform_setter.end(), [&] (auto& setter) { setter(shader); });

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
