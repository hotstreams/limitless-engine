#pragma once

#include <abstract_instance.hpp>
#include <model.hpp>

namespace GraphicsEngine {
    class ModelInstance : public AbstractInstance {
    protected:
        // vertices storage
        std::shared_ptr<AbstractModel> model;
        struct MeshInstance {
            MaterialInstance material;
            bool hidden {false};
        };
        std::unordered_map<std::string, MaterialInstance> materials;
        // model data
        glm::vec3 position, rotation, scale;
        glm::mat4 model_matrix;

        void calculateModelMatrix() noexcept;
    public:
        // model constructor
        ModelInstance(std::shared_ptr<AbstractModel> model, const glm::vec3& position);
        // elementary model constructor
        ModelInstance(std::shared_ptr<AbstractModel> model, const std::shared_ptr<Material>& material, const glm::vec3& position);

        void hide(uint32_t index);
        void reveal(uint32_t index);

        void setPosition(const glm::vec3& _position) noexcept;
        void setRotation(const glm::vec3& _rotation) noexcept;
        void setScale(const glm::vec3& _scale) noexcept;

        void draw(MaterialShaderType shader_type, Blending blending, const UniformSetter& uniform_setter = UniformSetter{}) override;
    };
}