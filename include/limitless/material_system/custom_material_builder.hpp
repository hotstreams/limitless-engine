#pragma once

#include <limitless/material_system/material_builder.hpp>
#include <limitless/material_system/custom_material.hpp>

namespace LimitlessEngine {
    class CustomMaterialBuilder : public MaterialBuilder {
    private:
        CustomMaterialBuilder& setUniforms(decltype(CustomMaterial::uniforms)&& uniforms);

        void setMaterialIndex() override;
        void checkRequirements() override;

        friend class MaterialSerializer;
    public:
        explicit CustomMaterialBuilder(Context& context, Assets& assets) : MaterialBuilder {context, assets} {}
        ~CustomMaterialBuilder() override = default;

        CustomMaterialBuilder& setFragmentCode(std::string fs_code) noexcept;
        CustomMaterialBuilder& setVertexCode(std::string vs_code) noexcept;
        CustomMaterialBuilder& setGlobalDefinitions(std::string global_code) noexcept;
        CustomMaterialBuilder& addUniform(std::string name, Uniform* uniform);
        CustomMaterialBuilder& removeUniform(const std::string& name);

        CustomMaterialBuilder& create(std::string name) override;
    };
}