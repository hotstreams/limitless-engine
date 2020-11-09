#pragma once

#include <material_system/material_builder.hpp>
#include <material_system/custom_material.hpp>

namespace GraphicsEngine {
    class CustomMaterialBuilder : public MaterialBuilder {
    public:
        CustomMaterialBuilder() = default;
        ~CustomMaterialBuilder() override = default;

        CustomMaterialBuilder& setFragmentCode(std::string fs_code) noexcept;
        CustomMaterialBuilder& setVertexCode(std::string vs_code) noexcept;
        CustomMaterialBuilder& addUniform(std::string name, Uniform* uniform);

        CustomMaterialBuilder& create(std::string name);
        virtual std::shared_ptr<Material> build(const ModelShaders& model_shaders = {ModelShader::Model}, const MaterialShaders& material_shaders = {MaterialShader::Default}) override;
    };
}