#pragma once

#include <material_system/material.hpp>

#include <stdexcept>

namespace LimitlessEngine {
    class CustomMaterial : public Material {
    private:
        std::unordered_map<std::string, std::unique_ptr<Uniform>> uniforms;
        std::string vertex_code;
        std::string fragment_code;

        void update() const noexcept override;

        friend class CustomMaterialBuilder;
        friend class MaterialCompiler;
        friend class EffectBuilder;
        friend class ShaderProgram;
        friend class MaterialSerializer;
        friend void swap(CustomMaterial& lhs, CustomMaterial& rhs) noexcept;
        CustomMaterial() noexcept;
    public:
        ~CustomMaterial() override = default;

        CustomMaterial(CustomMaterial&&) noexcept;
        CustomMaterial& operator=(CustomMaterial&&) noexcept;

        CustomMaterial(const CustomMaterial&);
        CustomMaterial& operator=(CustomMaterial);

        [[nodiscard]] CustomMaterial* clone() const noexcept override;

        bool isCustom() const noexcept override { return true; }

        const auto& getVertexCode() const noexcept { return vertex_code; }
        const auto& getFragmentCode() const noexcept { return fragment_code; }

        template<typename T>
        T& getUniform(const std::string& name) {
            try {
                return static_cast<T&>(*uniforms.at(name));
            } catch (const std::out_of_range& e) {
                throw std::runtime_error("No such uniform in custom material.");
            }
        }
    };

    void swap(CustomMaterial& lhs, CustomMaterial& rhs) noexcept;
}
