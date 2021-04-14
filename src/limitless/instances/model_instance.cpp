#include <limitless/instances/model_instance.hpp>

#include <limitless/core/shader_program.hpp>
#include <limitless/core/context.hpp>
#include <limitless/shader_storage.hpp>
#include <limitless/models/model.hpp>
#include <limitless/material_system/material.hpp>
#include <limitless/core/uniform_setter.hpp>
#include <limitless/assets.hpp>

using namespace LimitlessEngine;

ModelInstance::ModelInstance(decltype(model) _model, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale)
    : ModelInstance{nullptr, std::move(_model), _position, _rotation, _scale} {
}

ModelInstance::ModelInstance(Lighting* lighting, decltype(model) _model, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale)
    : AbstractInstance{lighting, ModelShader::Model, _position, _rotation, _scale}
    , model{std::move(_model)} {

    try {
        auto& simple_model = dynamic_cast<Model&>(*model);
        auto& model_meshes = simple_model.getMeshes();
        auto& model_mats = simple_model.getMaterials();

        for (uint32_t i = 0; i < model_meshes.size(); ++i) {
            meshes.emplace(model_meshes[i]->getName(), MeshInstance{model_meshes[i], model_mats[i]});
        }
    } catch (...) {
        throw std::runtime_error{"Wrong model for ModelInstance"};
    }
}

void ModelInstance::draw(Context& ctx, const Assets& assets, MaterialShader material_shader, Blending blending, const UniformSetter& uniform_setter) {
    if (hidden) {
        return;
    }

    if (!shadow_cast && material_shader == MaterialShader::DirectionalShadow) {
        return;
    }

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

                if (material->getTwoSided()) {
                    ctx.disable(Capabilities::CullFace);
                } else {
                    ctx.enable(Capabilities::CullFace);
                }

                // gets required shader from storage
                auto& shader = assets.shaders.get(material_shader, shader_type, material->getShaderIndex());

                // updates model/material uniforms
                shader << UniformValue{"model", model_matrix}
                       << *material;

                // sets custom pass-dependent uniforms
                uniform_setter(shader);

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

void ModelInstance::calculateBoundingBox() noexcept {
    bounding_box.center = glm::vec4{position, 1.0f} + glm::vec4{model->getBoundingBox().center, 1.0f} * model_matrix;
    bounding_box.size = glm::vec4{model->getBoundingBox().size, 1.0f} * model_matrix;
}
