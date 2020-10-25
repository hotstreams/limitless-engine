#pragma once

#include <abstract_instance.hpp>
#include <mesh_instance.hpp>
#include <glm/glm.hpp>
#include <string>

namespace GraphicsEngine {
    class AbstractModel;

    class ModelInstance : public AbstractInstance {
    protected:
        std::unordered_map<std::string, MeshInstance> meshes;
        std::shared_ptr<AbstractModel> model;
        glm::mat4 model_matrix;

        void calculateModelMatrix() noexcept;
    public:
        ModelInstance(decltype(model) model, const glm::vec3& position, const glm::vec3& rotation = glm::vec3{0.0f}, const glm::vec3& scale = glm::vec3{1.0f});
        ModelInstance(Lighting* lighting, decltype(model) model, const glm::vec3& position, const glm::vec3& rotation = glm::vec3{0.0f}, const glm::vec3& scale = glm::vec3{1.0f});

        ~ModelInstance() override = default;

        ModelInstance(const ModelInstance&) = default;
        ModelInstance& operator=(const ModelInstance&) = default;

        ModelInstance(ModelInstance&&) = default;
        ModelInstance& operator=(ModelInstance&&) = default;

        ModelInstance* clone() noexcept override;

        MeshInstance& operator[](const std::string& mesh);

        using AbstractInstance::draw;
        void draw(MaterialShader shader_type, Blending blending, const UniformSetter& uniform_setter) override;
    };
}