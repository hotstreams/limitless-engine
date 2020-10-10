#pragma once

#include <uniform.hpp>

#include <unordered_map>
#include <glm/glm.hpp>
#include <vector>
#include <map>

namespace GraphicsEngine {
    class Buffer;

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
        MetallicTexture,
        Roughness,
        RoughnessTexture
    };

    enum class Blending { Opaque, Translucent, Additive, Modulate };
    enum class Shading { Lit, Unlit };

    class Material {
    protected:
        std::map<PropertyType, std::unique_ptr<Uniform>> properties;
        std::unordered_map<std::string, uint64_t> uniform_offsets;
        std::shared_ptr<Buffer> material_buffer;
        Blending blending;
        Shading shading;
        std::string name;
        uint64_t shader_index;

        virtual void update() const noexcept;

        friend class ShaderProgram;
        friend class MaterialBuilder;
        friend void swap(Material& lhs, Material& rhs) noexcept;
        Material() noexcept;
        Material(decltype(properties)&& properties, decltype(uniform_offsets)&& offsets, Blending blending, Shading shading, std::string name, uint64_t shader_index) noexcept;
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
        Blending blending;
        Shading shading;
    };
    bool operator<(const MaterialType& lhs, const MaterialType& rhs) noexcept;
}