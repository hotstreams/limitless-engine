#include <model_instance.hpp>

#include <elementary_model.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace GraphicsEngine;

void AbstractInstance::reveal() noexcept {
    hidden = false;
}

void AbstractInstance::hide() noexcept {
    hidden = true;
}

void AbstractInstance::kill() noexcept {
    done = true;
}

bool AbstractInstance::isKilled() const noexcept {
    return done;
}

void AbstractInstance::asWireFrame() noexcept {
    wireframe = true;
}

void AbstractInstance::asModel() noexcept {
    wireframe = false;
}

bool AbstractInstance::isWireFrame() const noexcept {
    return wireframe;
}

ModelInstance::ModelInstance(std::shared_ptr<AbstractModel> m, const glm::vec3& position)
    : AbstractInstance{ModelShaderType::Model}, model{std::move(m)}, position{position}, rotation{0.0f}, scale{1.0f}, model_matrix{1.0f} {
        auto& simple_model = static_cast<Model&>(*model);

        auto& model_meshes = simple_model.getMeshes();
        auto& model_mats = simple_model.getMaterials();

        for(uint32_t i = 0; i < model_meshes.size(); ++i) {
            meshes.emplace(model_meshes[i]->getName(), MeshInstance{model_meshes[i], model_mats[i]});
        }
}

ModelInstance::ModelInstance(std::shared_ptr<AbstractModel> m, const std::shared_ptr<Material>& material, const glm::vec3& position)
    : AbstractInstance{ModelShaderType::Model}, model{std::move(m)}, position{position}, rotation{0.0f}, scale{1.0f}, model_matrix{1.0f} {
        auto& elementary = static_cast<ElementaryModel&>(*model);

        meshes.emplace(elementary.getMesh()->getName(), MeshInstance{elementary.getMesh(), material});
}

void ModelInstance::calculateModelMatrix() noexcept {
    model_matrix = glm::mat4{1.0f};

    model_matrix = glm::translate(model_matrix, position);

    model_matrix = glm::rotate(model_matrix, rotation.x, glm::vec3(1.0f, 0.f, 0.f));
    model_matrix = glm::rotate(model_matrix, rotation.y, glm::vec3(0.0f, 1.f, 0.f));
    model_matrix = glm::rotate(model_matrix, rotation.z, glm::vec3(0.0f, 0.f, 1.f));

    model_matrix = glm::scale(model_matrix, scale);
}

void ModelInstance::setPosition(const glm::vec3& _position) noexcept {
    position = _position;
}

void ModelInstance::setRotation(const glm::vec3& _rotation) noexcept {
    rotation = _rotation;
}

void ModelInstance::setScale(const glm::vec3& _scale) noexcept {
    scale = _scale;
}

void ModelInstance::draw(MaterialShaderType material_type, Blending blending, const UniformSetter& uniform_setter) {
    if (hidden) {
        return;
    }

    calculateModelMatrix();

    for (const auto& [name, mesh] : meshes) {
        if (mesh.isHidden()) {
            continue;
        }

        const auto& material = mesh.getMaterial().get();

        if (material.getBlending() == blending) {
            auto& shader = shader_storage.get(material_type, type, material.getShaderIndex());

            *shader << material
                    << UniformValue{"model", model_matrix};

            for (const auto& uniform_set : uniform_setter) {
                uniform_set(*shader);
            }

            shader->use();

            mesh.draw();
        }
    }
}

void ModelInstance::hide(const std::string& name) {
    try {
        meshes.at(name).hide();
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No such mesh name.");
    }
}

void ModelInstance::reveal(const std::string& name) {
    try {
        meshes.at(name).reveal();
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No such mesh name.");
    }
}
