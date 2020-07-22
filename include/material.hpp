#pragma once

#include <buffer_builder.hpp>
#include <uniform.hpp>

namespace GraphicsEngine {
    enum class PropertyType {
        Color,
        EmissiveColor,
        Diffuse,
        Specular,
        Normal,
        Displacement,
        EmissiveMask,
        BlendMask,
        Shininess,
        Metallic,
        Roughness
    };
    enum class Blending { Opaque, Translucent, Additive };
    enum class Shading { Lit, Unlit };

    class Material {
    protected:
        std::map<PropertyType, std::unique_ptr<Uniform>> properties;
        std::shared_ptr<Buffer> material_buffer;
        Blending blending;
        Shading shading;
        std::string name;
        uint64_t shader_index;

        void update() const noexcept;

        friend class ShaderProgram;
        friend class MaterialBuilder;
        Material(decltype(properties)&& properties, Blending blending, Shading shading, std::string name, uint64_t shader_index) noexcept;
    public:
        [[nodiscard]] UniformValue<glm::vec4>& getColor() const;
        [[nodiscard]] UniformValue<glm::vec4>& getEmissiveColor() const;
        [[nodiscard]] UniformValue<float>& getShininess() const;
        [[nodiscard]] UniformValue<float>& getMetallic() const;
        [[nodiscard]] UniformValue<float>& getRoughness() const;
        [[nodiscard]] UniformSampler& getDiffuse() const;
        [[nodiscard]] UniformSampler& getSpecular() const;
        [[nodiscard]] UniformSampler& getNormal() const;
        [[nodiscard]] UniformSampler& getDisplacement() const;
        [[nodiscard]] UniformSampler& getEmissiveMask() const;
        [[nodiscard]] UniformSampler& getBlendMask() const;

        [[nodiscard]] const auto& getBlending() const noexcept { return blending; }
        [[nodiscard]] auto& getMaterialBuffer() const noexcept { return material_buffer; }
        [[nodiscard]] const auto& getShaderIndex() const noexcept { return shader_index; }
        [[nodiscard]] const auto& getName() const noexcept { return name; }

        Material(const Material& material);
        virtual ~Material() = default;
    };

    struct MaterialType {
        std::vector<PropertyType> properties;
        Blending blending;
        Shading shading;
    };

    bool operator<(const MaterialType& lhs, const MaterialType& rhs) noexcept;
}