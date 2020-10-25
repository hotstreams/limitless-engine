#include <elementary_instance.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <elementary_model.hpp>
#include <shader_storage.hpp>

using namespace GraphicsEngine;

void ElementaryInstance::calculateModelMatrix() noexcept {
    model_matrix = glm::mat4{1.0f};

    model_matrix = glm::translate(model_matrix, position);

    model_matrix = glm::rotate(model_matrix, rotation.x, glm::vec3{1.0f, 0.f, 0.f});
    model_matrix = glm::rotate(model_matrix, rotation.y, glm::vec3{0.0f, 1.f, 0.f});
    model_matrix = glm::rotate(model_matrix, rotation.z, glm::vec3{0.0f, 0.f, 1.f});

    model_matrix = glm::scale(model_matrix, scale);
}

ElementaryInstance::ElementaryInstance(decltype(model) _model, std::shared_ptr<Material> material, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale)
        : AbstractInstance{ModelShader::Model, _position, _rotation, _scale}, model{std::move(_model)}, mesh{dynamic_cast<ElementaryModel&>(*model).getMesh(), material}, model_matrix{1.0f} {
}

ElementaryInstance::ElementaryInstance(Lighting *lighting, decltype(model) _model, std::shared_ptr<Material> material, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale)
        : AbstractInstance{lighting, ModelShader::Model, _position, _rotation, _scale}, model{std::move(_model)}, mesh{dynamic_cast<ElementaryModel&>(*model).getMesh(), material}, model_matrix{1.0f} {
}

ElementaryInstance *ElementaryInstance::clone() noexcept {
    return new ElementaryInstance(*this);
}

void ElementaryInstance::draw(MaterialShader material_type, Blending blending, const AbstractInstance::UniformSetter& uniform_setter) {
    if (hidden) {
        return;
    }

    calculateModelMatrix();

    if (mesh.isHidden()) {
        return ;
    }

    auto materials = mesh.getMaterial().getMaterials();

    for (const auto& material : materials) {
        if (material->getBlending() == blending) {
            auto& shader = shader_storage.get(material_type, shader_type, material->getShaderIndex());

            *shader << *material
                    << UniformValue{"model", model_matrix};

            for (const auto& uniform_set : uniform_setter) {
                uniform_set(*shader);
            }

            shader->use();

            mesh.draw();
        }
    }
}
