#pragma once

#include <limitless/instances/instance.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/models/elementary_model.hpp>

namespace Limitless {
    class DecalInstance : public Instance {
    private:
        std::shared_ptr<AbstractModel> model;
        std::shared_ptr<ms::Material> material;
    public:
        DecalInstance(std::shared_ptr<AbstractModel> model, std::shared_ptr<ms::Material> material, const glm::vec3& position);

        DecalInstance(const DecalInstance&);

        void updateBoundingBox() noexcept override;
        std::unique_ptr<Instance> clone() noexcept override;

        void draw(Context& ctx, const Assets& assets, ShaderType shader_type, ms::Blending blending, const UniformSetter& uniform_set) override;
    };
}