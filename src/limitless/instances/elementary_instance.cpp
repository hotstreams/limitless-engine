#include <limitless/instances/elementary_instance.hpp>

#include <limitless/models/elementary_model.hpp>
#include <limitless/shader_storage.hpp>
#include <limitless/core/shader_program.hpp>
#include <limitless/assets.hpp>
#include <limitless/core/context.hpp>
#include <limitless/core/uniform_setter.hpp>
#include <limitless/ms/material.hpp>

using namespace Limitless;
using namespace Limitless::ms;

ElementaryInstance::ElementaryInstance(decltype(model) _model, std::shared_ptr<Material> material, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale)
    : AbstractInstance {ModelShader::Model, _position, _rotation, _scale}
    , model {std::move(_model)}
    , mesh {dynamic_cast<ElementaryModel&>(*model).getMesh(), std::move(material)} {
}

ElementaryInstance::ElementaryInstance(Lighting *lighting, decltype(model) _model, std::shared_ptr<Material> material, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale)
    : AbstractInstance {lighting, ModelShader::Model, _position, _rotation, _scale}
    , model {std::move(_model)}
    , mesh{dynamic_cast<ElementaryModel&>(*model).getMesh(), std::move(material)} {
}

ElementaryInstance *ElementaryInstance::clone() noexcept {
    return new ElementaryInstance(*this);
}

void ElementaryInstance::draw(Context& ctx, const Assets& assets, ShaderPass pass, Blending blending, const UniformSetter& uniform_setter) {
    if (hidden) {
        return;
    }

    calculateModelMatrix();

    mesh.draw(ctx, assets, pass, shader_type, model_matrix, blending, uniform_setter);
}

void ElementaryInstance::calculateBoundingBox() noexcept {
    bounding_box.center = glm::vec4{position, 1.0f} + glm::vec4{model->getBoundingBox().center, 1.0f} * model_matrix;
    bounding_box.size = glm::vec4{model->getBoundingBox().size, 1.0f} * model_matrix;
}
