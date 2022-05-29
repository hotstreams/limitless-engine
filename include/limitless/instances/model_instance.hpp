#pragma once

#include <limitless/instances/abstract_instance.hpp>
#include <limitless/instances/mesh_instance.hpp>

namespace Limitless {
    class AbstractModel;

    class ModelInstance : public AbstractInstance {
    protected:
        std::unordered_map<std::string, MeshInstance> meshes;
        std::shared_ptr<AbstractModel> model;

        void updateBoundingBox() noexcept override;
        ModelInstance(ModelShader shader, decltype(model) model, const glm::vec3& position);
    public:
        // model constructor
        ModelInstance(decltype(model) model, const glm::vec3& position = glm::vec3{0.0f});

        // elementary model constructor
        ModelInstance(decltype(model) model, std::shared_ptr<ms::Material> material, const glm::vec3& position = glm::vec3{0.0f});

        ~ModelInstance() override = default;

        ModelInstance(const ModelInstance&) = default;
        ModelInstance(ModelInstance&&) noexcept = default;

        void update(Context &context, const Camera &camera) override;

        ModelInstance* clone() noexcept override;

        const auto& getAbstractModel() const noexcept { return *model; }

        MeshInstance& operator[](const std::string& mesh);

        const auto& getMeshes() const noexcept { return meshes; }
        auto& getMeshes() noexcept { return meshes; }

        using AbstractInstance::draw;
        void draw(Context& ctx, const Assets& assets, ShaderPass shader_type, ms::Blending blending, const UniformSetter& uniform_setter) override;
    };
}