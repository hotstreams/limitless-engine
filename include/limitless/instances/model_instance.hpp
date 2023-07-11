#pragma once

#include <limitless/instances/abstract_instance.hpp>
#include <limitless/instances/mesh_instance.hpp>

namespace Limitless {
    class AbstractModel;

    class no_such_mesh : public std::runtime_error {
    private:
        using std::runtime_error::runtime_error;
    };
}

namespace Limitless {
    /**
     * ModelInstance is an instance that contains static model
     */
    class ModelInstance : public AbstractInstance {
    protected:
        //TODO: material applying interface to all meshes
        std::map<std::string, MeshInstance> meshes;
        std::shared_ptr<AbstractModel> model;

        void updateBoundingBox() noexcept override;
        ModelInstance(InstanceType shader, decltype(model) model, const glm::vec3& position);
    public:
        /**
         * Creates static model with specified loaded Model
         */
        explicit ModelInstance(decltype(model) model, const glm::vec3& position = glm::vec3{0.0f});

        /**
         * Creates static model with specified elementary model and material
         */
        ModelInstance(decltype(model) model, std::shared_ptr<ms::Material> material, const glm::vec3& position = glm::vec3{0.0f});

        ~ModelInstance() override = default;

        ModelInstance(const ModelInstance&) = default;
        ModelInstance(ModelInstance&&) noexcept = default;

        /**
         * Updates instance and meshes
         */
        void update(Context &context, const Camera &camera) override;

        /**
         * Makes copy
         */
        std::unique_ptr<AbstractInstance> clone() noexcept override;

        /**
         * Gets used model
         */
        const auto& getAbstractModel() const noexcept { return *model; }

        /**
         * Gets mesh instance with specified name
         *
         * throws no_such_mesh if not found
         */
        MeshInstance& operator[](const std::string& mesh);

        /**
         * Gets mesh instance with specified index
         *
         * throws no_such_mesh if not found
         */
        MeshInstance& operator[](uint32_t index);

        const auto& getMeshes() const noexcept { return meshes; }
        auto& getMeshes() noexcept { return meshes; }

        using AbstractInstance::draw;
        void draw(Context& ctx, const Assets& assets, ShaderType shader_type, ms::Blending blending, const UniformSetter& uniform_setter) override;
    };
}