#pragma once

#include <abstract_instance.hpp>
#include <model.hpp>
#include <mesh_instance.hpp>

namespace GraphicsEngine {
    class ModelInstance : public AbstractInstance {
    protected:
        std::shared_ptr<AbstractModel> model;
        std::unordered_map<std::string, MeshInstance> meshes;
        // model data
        glm::vec3 position, rotation, scale;
        glm::mat4 model_matrix;

        void calculateModelMatrix() noexcept;
    public:
        // model constructor
        ModelInstance(std::shared_ptr<AbstractModel> model, const glm::vec3& position);
        // elementary model constructor
        ModelInstance(std::shared_ptr<AbstractModel> model, const std::shared_ptr<Material>& material, const glm::vec3& position);

        void hide(const std::string& name);
        void reveal(const std::string& name);

        ModelInstance& setPosition(const glm::vec3& _position) noexcept;
        ModelInstance& setRotation(const glm::vec3& _rotation) noexcept;
        ModelInstance& setScale(const glm::vec3& _scale) noexcept;

        void draw(MaterialShaderType shader_type, Blending blending, const UniformSetter& uniform_setter = UniformSetter{}) override;
    };
}