#pragma once

#include <material.hpp>
#include <functional>
#include <shader_types.hpp>

namespace GraphicsEngine {
    class MaterialBuilder {
    protected:
        static inline uint64_t next_shader_index {0};
        static inline std::map<MaterialType, uint64_t> unique_materials;

        std::map<PropertyType, std::unique_ptr<Uniform>> properties;
        std::unordered_map<std::string, uint64_t> uniform_offsets;
        Blending blending {Blending::Opaque};
        Shading shading {Shading::Unlit};
        uint64_t material_index {0};
        size_t buffer_size {0};

        void getUniformAlignments(const std::shared_ptr<ShaderProgram>& shader) noexcept;
        virtual void clear() noexcept;
    public:
        MaterialBuilder() noexcept = default;
        virtual ~MaterialBuilder() = default;

        MaterialBuilder& add(PropertyType type, std::shared_ptr<Texture> texture);
        MaterialBuilder& add(PropertyType type, const glm::vec4& value);
        MaterialBuilder& add(PropertyType type, float value);
        MaterialBuilder& setBlending(Blending blending) noexcept;
        MaterialBuilder& setShading(Shading shading) noexcept;

        [[nodiscard]] MaterialType getMaterialType() const noexcept;
        [[nodiscard]] MaterialType getMaterialType(const Material& material) const noexcept;
        [[nodiscard]] auto getMaterialIndex() const noexcept { return material_index; }

        virtual std::shared_ptr<Material> build(const std::string& name,
                                                const RequiredModelShaders& model_shaders = RequiredModelShaders{ ModelShader::Model },
                                                const RequiredMaterialShaders& material_shaders = RequiredMaterialShaders{ MaterialShader::Default });
    };
}