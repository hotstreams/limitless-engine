#pragma once

#include <limitless/material_system/material_builder.hpp>
#include <limitless/material_system/custom_material.hpp>
#include <limitless/core/context.hpp>

namespace LimitlessEngine {
    class CustomMaterialBuilder : public MaterialBuilder {
    private:
        CustomMaterialBuilder& setUniforms(decltype(CustomMaterial::uniforms)&& uniforms);
        friend class MaterialSerializer;
    public:
        explicit CustomMaterialBuilder(Context& context, Assets& assets) : MaterialBuilder {context, assets} {}
        ~CustomMaterialBuilder() override = default;

        CustomMaterialBuilder& setFragmentCode(std::string fs_code) noexcept;
        CustomMaterialBuilder& setVertexCode(std::string vs_code) noexcept;
        CustomMaterialBuilder& addUniform(std::string name, Uniform* uniform);

        CustomMaterialBuilder& create(std::string name) override;
        std::shared_ptr<Material> build(const ModelShaders& model_shaders = {ModelShader::Model},
                                        const MaterialShaders& material_shaders = {MaterialShader::Forward, MaterialShader::DirectionalShadow}) override;
    };
}