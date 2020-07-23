#pragma once

#include <material.hpp>
#include <functional>
#include <shader_storage.hpp>

namespace GraphicsEngine {
    class MaterialBuilder {
    private:
        static inline uint64_t shader_index {0};
        static inline std::map<MaterialType, uint64_t> unique_materials;

        std::map<PropertyType, std::unique_ptr<Uniform>> properties;
        Blending blending {Blending::Opaque};
        Shading shading {Shading::Unlit};

        [[nodiscard]] MaterialType getMaterialType() const noexcept;
        [[nodiscard]] size_t getMaterialBufferSize() const noexcept;
    public:
        std::shared_ptr<Material> build(const std::string& name, const RequiredModelShaders& model_shaders = RequiredModelShaders{ ModelShaderType::Model },
                                                                 const RequiredMaterialShaders& material_shaders = RequiredMaterialShaders{ MaterialShaderType::Default });

        MaterialBuilder& add(PropertyType type, std::shared_ptr<Texture> texture);
        MaterialBuilder& add(PropertyType type, const glm::vec4& value);
        MaterialBuilder& add(PropertyType type, float value);
        MaterialBuilder& setBlending(Blending blending) noexcept;
        MaterialBuilder& setShading(Shading shading) noexcept;
    };
}