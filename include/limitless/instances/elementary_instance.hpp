#pragma once

#include <limitless/instances/model_instance.hpp>

namespace Limitless {
    class ElementaryInstance : public AbstractInstance {
    protected:
        std::shared_ptr<AbstractModel> model;
        MeshInstance mesh;

        void calculateBoundingBox() noexcept override;
    public:
        ElementaryInstance(decltype(model) model, std::shared_ptr<ms::Material> material, const glm::vec3& position, const glm::vec3& rotation = glm::vec3{0.0f}, const glm::vec3& scale = glm::vec3{1.0f});
        ElementaryInstance(Lighting* lighting, decltype(model) model, std::shared_ptr<ms::Material> material, const glm::vec3& position, const glm::vec3& rotation = glm::vec3{0.0f}, const glm::vec3& scale = glm::vec3{1.0f});

        ~ElementaryInstance() override = default;

        ElementaryInstance(const ElementaryInstance&) = default;
        ElementaryInstance(ElementaryInstance&&) noexcept = default;

        [[nodiscard]] ElementaryInstance* clone() noexcept override;

        [[nodiscard]] MeshInstance& getMesh() noexcept { return mesh; }
        [[nodiscard]] const MeshInstance& getMesh() const noexcept { return mesh; }

        using AbstractInstance::draw;
        void draw(Context& ctx, const Assets& assets, ShaderPass shader_type, ms::Blending blending, const UniformSetter& uniform_setter) override;
    };
}