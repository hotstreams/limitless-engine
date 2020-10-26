#pragma once

#include <material_system/material.hpp>
#include <functional>
#include <shader_types.hpp>

namespace GraphicsEngine {
    class MaterialBuilder {
    private:
        static inline std::map<MaterialType, uint64_t> unique_materials;
        std::unique_ptr<Material> material;

        [[nodiscard]] MaterialType getMaterialType() const noexcept;
    protected:
        static inline uint64_t next_shader_index {0};

        void initializeMaterialBuffer(const ShaderProgram& shader) noexcept;
    public:
        MaterialBuilder() = default;
        virtual ~MaterialBuilder() = default;

        MaterialBuilder& add(PropertyType type, std::shared_ptr<Texture> texture);
        MaterialBuilder& add(PropertyType type, const glm::vec4& value);
        MaterialBuilder& setBlending(Blending blending) noexcept;
        MaterialBuilder& setShading(Shading shading) noexcept;
        MaterialBuilder& add(PropertyType type, float value);

        MaterialBuilder& create(std::string name);
        std::shared_ptr<Material> build(const ModelShaders& model_shaders = {ModelShader::Model}, const MaterialShaders& material_shaders = {MaterialShader::Default});
    };
}