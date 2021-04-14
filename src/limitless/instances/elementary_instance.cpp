#include <limitless/instances/elementary_instance.hpp>

#include <limitless/models/elementary_model.hpp>
#include <limitless/shader_storage.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/context_state.hpp>
#include <limitless/core/uniform_setter.hpp>
#include <limitless/material_system/material.hpp>

using namespace LimitlessEngine;

ElementaryInstance::ElementaryInstance(decltype(model) _model, std::shared_ptr<Material> material, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale)
        : AbstractInstance{ModelShader::Model, _position, _rotation, _scale}, model{std::move(_model)}, mesh{dynamic_cast<ElementaryModel&>(*model).getMesh(), material} {
}

ElementaryInstance::ElementaryInstance(Lighting *lighting, decltype(model) _model, std::shared_ptr<Material> material, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale)
        : AbstractInstance{lighting, ModelShader::Model, _position, _rotation, _scale}, model{std::move(_model)}, mesh{dynamic_cast<ElementaryModel&>(*model).getMesh(), material} {
}

ElementaryInstance *ElementaryInstance::clone() noexcept {
    return new ElementaryInstance(*this);
}

void ElementaryInstance::draw(Context& ctx, const Assets& assets, MaterialShader material_type, Blending blending, const UniformSetter& uniform_setter) {
    if (hidden) {
        return;
    }

    if (mesh.isHidden()) {
        return;
    }

    if (!shadow_cast && material_type == MaterialShader::DirectionalShadow) {
        return;
    }

    calculateModelMatrix();

    auto first_opaque {true};
    for (const auto& [layer, material] : mesh.getMaterial()) {
        if (material->getBlending() == blending) {
            if (blending == Blending::Opaque && mesh.getMaterial().count() > 1 && !first_opaque) {
                setBlendingMode(Blending::OpaqueHalf);
            } else {
                setBlendingMode(blending);
                if (blending == Blending::Opaque) first_opaque = false;
            }

            if (auto* state = ContextState::getState(glfwGetCurrentContext()); material->getTwoSided()) {
                state->disable(Capabilities::CullFace);
            } else {
                state->enable(Capabilities::CullFace);
            }

            auto& shader = assets.shaders.get(material_type, shader_type, material->getShaderIndex());

            shader << *material
                   << UniformValue{"model", model_matrix};

            uniform_setter(shader);

            shader.use();

            mesh.draw();
        }
    }
}

void ElementaryInstance::calculateBoundingBox() noexcept {
    bounding_box.center = glm::vec4{position, 1.0f} + glm::vec4{model->getBoundingBox().center, 1.0f} * model_matrix;
    bounding_box.size = glm::vec4{model->getBoundingBox().size, 1.0f} * model_matrix;
}
