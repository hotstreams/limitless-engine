#pragma once

#include <material_builder.hpp>
#include <custom_material.hpp>

namespace GraphicsEngine {
    class CustomMaterialBuilder : public MaterialBuilder {
    private:
        std::unordered_map<std::string, std::unique_ptr<Uniform>> uniforms;
        std::string vertex_code, fragment_code;

        void clear() noexcept override;
    public:
        CustomMaterialBuilder() noexcept = default;
        ~CustomMaterialBuilder() override = default;

        void addUniform(std::string name, Uniform* uniform);

        void setVertexCode(const std::string& vs_code) noexcept;
        void setFragmentCode(const std::string& fs_code) noexcept;

        [[nodiscard]] const auto& getVertexCode() const noexcept { return vertex_code; }
        [[nodiscard]] const auto& getFragmentCode() const noexcept { return fragment_code; }
        [[nodiscard]] const auto& getUniforms() const noexcept { return uniforms; }

        std::shared_ptr<Material> build(const std::string& name,
                                        const RequiredModelShaders& model_shaders = RequiredModelShaders{ModelShader::Model },
                                        const RequiredMaterialShaders& material_shaders = RequiredMaterialShaders{MaterialShader::Default }) override;
    };
}