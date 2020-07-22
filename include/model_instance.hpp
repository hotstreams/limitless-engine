#pragma once

#include <shader_storage.hpp>
#include <material.hpp>
#include <functional>
#include <material_instance.hpp>
#include <model.hpp>

namespace GraphicsEngine {
    class AbstractInstance {
    private:
        static inline uint64_t next_id {0};
        uint64_t id;
    protected:
        bool done {false};
        bool hidden {false};
        bool wireframe {false};
    public:
        AbstractInstance() noexcept : id{next_id++} { }
        [[nodiscard]] const auto& getId() const noexcept { return id; }

        void reveal() noexcept;
        void hide() noexcept;

        void kill() noexcept;
        [[nodiscard]] bool isKilled() const noexcept;

        void asWireFrame() noexcept;
        void asModel() noexcept;
        [[nodiscard]] bool isWireFrame() const noexcept;

        using UniformSetter = std::vector<std::function<void(ShaderProgram&)>>;
        virtual void draw(MaterialShaderType shader_type, Blending blending, const UniformSetter& uniform_set = UniformSetter{}) = 0;
    };

    class ModelInstance : public AbstractInstance {
    private:
        // vertices storage
        std::shared_ptr<AbstractModel> model;
        // [mesh_name, material]
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

        void setPosition(const glm::vec3& _position) noexcept;
        void setRotation(const glm::vec3& _rotation) noexcept;
        void setScale(const glm::vec3& _scale) noexcept;

        void draw(MaterialShaderType shader_type, Blending blending, const UniformSetter& uniform_setter = UniformSetter{}) override;
    };
}