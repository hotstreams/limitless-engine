#pragma once

#include <limitless/core/uniform.hpp>

#include <limitless/material_system/properties.hpp>
#include <limitless/shader_types.hpp>
#include <unordered_map>
#include <glm/glm.hpp>
#include <vector>
#include <map>

namespace LimitlessEngine {
    class Buffer;

    class Material {
    protected:
        std::map<PropertyType, std::unique_ptr<Uniform>> properties;
        std::unordered_map<std::string, uint64_t> uniform_offsets;
        std::shared_ptr<Buffer> material_buffer;
        Blending blending;
        Shading shading;
        std::string name;
        uint64_t shader_index;

        ModelShaders model_shaders;

        virtual void update() const noexcept;

        friend class ShaderProgram;
        friend class MaterialBuilder;
        friend class CustomMaterialBuilder;
        friend class EffectBuilder;
        friend class MaterialCompiler;
        friend class MaterialSerializer;
        friend void swap(Material& lhs, Material& rhs) noexcept;
        Material() noexcept;
    public:
        // gets specified property parameter
        [[nodiscard]] UniformValue<glm::vec4>& getColor() const;
        [[nodiscard]] UniformValue<glm::vec4>& getEmissiveColor() const;
        [[nodiscard]] UniformValue<float>& getShininess() const;
        [[nodiscard]] UniformValue<float>& getMetallic() const;
        [[nodiscard]] UniformValue<float>& getRoughness() const;
        [[nodiscard]] UniformSampler& getMetallicTexture() const;
        [[nodiscard]] UniformSampler& getRoughnessTexture() const;
        [[nodiscard]] UniformSampler& getDiffuse() const;
        [[nodiscard]] UniformSampler& getSpecular() const;
        [[nodiscard]] UniformSampler& getNormal() const;
        [[nodiscard]] UniformSampler& getDisplacement() const;
        [[nodiscard]] UniformSampler& getEmissiveMask() const;
        [[nodiscard]] UniformSampler& getBlendMask() const;

        [[nodiscard]] auto getShaderIndex() const noexcept { return shader_index; }
        [[nodiscard]] auto& getBlending() noexcept { return blending; }
        [[nodiscard]] const auto& getName() const noexcept { return name; }
        [[nodiscard]] virtual bool isCustom() const noexcept { return false; }
        [[nodiscard]] const auto& getModelShaders() const noexcept { return model_shaders; }

        Material(Material&&) noexcept;
        Material& operator=(Material&&) noexcept;

        Material(const Material&) noexcept;
        Material& operator=(Material) noexcept;

        [[nodiscard]] virtual Material* clone() const noexcept;

        virtual ~Material() = default;
    };

    void swap(Material& lhs, Material& rhs) noexcept;

    struct MaterialType {
        std::vector<PropertyType> properties;
        Shading shading;
    };
    bool operator<(const MaterialType& lhs, const MaterialType& rhs) noexcept;
}