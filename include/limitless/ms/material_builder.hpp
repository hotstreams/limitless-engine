#pragma once

#include <limitless/ms/material.hpp>
#include <limitless/pipeline/shader_type.hpp>
#include <limitless/ms/unique_material.hpp>
#include <functional>
#include <mutex>
#include <limitless/renderer/render_settings.hpp>

namespace Limitless {
    class Assets;
}

namespace Limitless::ms {
    class material_builder_error : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class Material::Builder {
    private:
        static inline std::map<UniqueMaterial, uint64_t> unique_materials;
        static inline uint64_t next_shader_index {};
        static inline std::mutex mutex;

        std::shared_ptr<Material> material;

        Assets& assets;

        [[nodiscard]] UniqueMaterial getMaterialType() const noexcept;
        void initializeMaterialBuffer();
        void checkRequirements();
        void setMaterialIndex();
        void setModelShaders();
        void createMaterial();
    public:
        explicit MaterialBuilder(Assets& assets);
        virtual ~MaterialBuilder() = default;

        MaterialBuilder(const MaterialBuilder&) = delete;
        MaterialBuilder& operator=(const MaterialBuilder&) = delete;

        MaterialBuilder(MaterialBuilder&&) = delete;
        MaterialBuilder& operator=(MaterialBuilder&&) = delete;

        Builder& color(const glm::vec4& color) noexcept;
        Builder& emissive_color(const glm::vec4& color) noexcept;
        Builder& diffuse(const glm::vec4& color) noexcept;
        Builder& normal(const glm::vec4& color) noexcept;
        Builder& emissive_mask(const glm::vec4& color) noexcept;
        Builder& blend_mask(const glm::vec4& color) noexcept;
        Builder& metallic(const glm::vec4& color) noexcept;
        Builder& metallic(const glm::vec4& color) noexcept;
        Builder& roughness(const glm::vec4& color) noexcept;
        Builder& roughness(const glm::vec4& color) noexcept;
        Builder& ao(const glm::vec4& color) noexcept;
        Builder& orm(const glm::vec4& color) noexcept;
        Builder& ior(const glm::vec4& color) noexcept;
        Builder& absorption(const glm::vec4& color) noexcept;
        Builder& microthickness(const glm::vec4& color) noexcept;
        Builder& reflectance(const glm::vec4& color) noexcept;
        Builder& transmission(const glm::vec4& color) noexcept;

        Builder& blending(const glm::vec4& color) noexcept;
        Builder& shading(const glm::vec4& color) noexcept;
        Builder& two_sided(const glm::vec4& color) noexcept;
        Builder& refraction(const glm::vec4& color) noexcept;
        Builder& name(const glm::vec4& color) noexcept;

        Builder& vertex(const glm::vec4& color) noexcept;
        Builder& fragment(const glm::vec4& color) noexcept;
        Builder& global(const glm::vec4& color) noexcept;
        Builder& shading(const glm::vec4& color) noexcept;
        Builder& model(const glm::vec4& color) noexcept;
        Builder& models(const glm::vec4& color) noexcept;





        MaterialBuilder& add(Property type, std::shared_ptr<Texture> texture);
        MaterialBuilder& add(Property type, const glm::vec4& value);
        MaterialBuilder& add(Property type, const glm::vec2& value);
        MaterialBuilder& add(Property type, float value);
        MaterialBuilder& remove(Property type);

        MaterialBuilder& setBlending(Blending blending) noexcept;
        MaterialBuilder& setShading(Shading shading) noexcept;
        MaterialBuilder& setTwoSided(bool two_sided) noexcept;
        MaterialBuilder& setRefraction(bool refraction) noexcept;
        MaterialBuilder& setName(std::string name) noexcept;
        [[nodiscard]] const auto& getName() const noexcept { return material->name; }

        MaterialBuilder& setFragmentSnippet(std::string fs_code) noexcept;
        MaterialBuilder& setVertexSnippet(std::string vs_code) noexcept;
        MaterialBuilder& setTessellationSnippet(std::string tes_code) noexcept;
        MaterialBuilder& setGlobalSnippet(std::string global_code) noexcept;

        MaterialBuilder& addUniform(std::unique_ptr<Uniform> uniform);
        MaterialBuilder& removeUniform(const std::string& name);

        MaterialBuilder& setModelShaders(const InstanceTypes& shaders) noexcept;
        MaterialBuilder& addModelShader(InstanceType model) noexcept;

        MaterialBuilder& set(decltype(material->properties)&& properties);
        MaterialBuilder& set(decltype(material->uniforms)&& uniforms);

        void clear();
        void setTo(const std::shared_ptr<Material>& material);
        std::shared_ptr<Material> build();
        std::shared_ptr<Material> buildSkybox();
    };
}