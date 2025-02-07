#pragma once

#include <limitless/ms/material.hpp>
#include <limitless/ms/unique_material.hpp>
#include <mutex>

namespace Limitless {
    class Assets;
}

namespace Limitless::ms {
    class material_builder_exception : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class Material::Builder {
    private:
        static inline std::map<UniqueMaterial, uint64_t> unique_materials;
        static inline uint64_t next_shader_index {1};
        static inline std::mutex mutex;

        std::map<Property, std::unique_ptr<Uniform>> properties;
        std::map<std::string, std::unique_ptr<Uniform>> uniforms;

        Blending _blending {Blending::Opaque};
        Shading _shading {Shading::Lit};
        bool _two_sided {false};
        bool _refraction {false};
        std::string _name;
        uint64_t shader_index {};
        InstanceTypes _model_shaders;

        std::string vertex_snippet;
        std::string fragment_snippet;
        std::string global_fragment_snippet;
        std::string global_vertex_snippet;
        std::string shading_snippet;

        bool normal_map_ {false};
        bool orm_map_ {false};

        bool _skybox {false};

        UniqueMaterial getMaterialType() const noexcept;
        void checkRequirements();
        void setMaterialIndex();
        void setModelShaders();

        friend class Material;
    public:
        Builder() noexcept = default;
        virtual ~Builder() = default;

        Builder(const Builder&) = delete;
        Builder& operator=(const Builder&) = delete;

        Builder(Builder&&) = delete;
        Builder& operator=(Builder&&) = delete;

        Builder& color(const glm::vec4& color) noexcept;
        Builder& emissive_color(const glm::vec3& emissive_color) noexcept;
        Builder& diffuse(const std::shared_ptr<Texture>& texture) noexcept;
        Builder& normal(const std::shared_ptr<Texture>& texture) noexcept;
        Builder& emissive_mask(const std::shared_ptr<Texture>& texture) noexcept;
        Builder& blend_mask(const std::shared_ptr<Texture>& texture) noexcept;
        Builder& metallic(float metallic) noexcept;
        Builder& metallic(const std::shared_ptr<Texture>& texture) noexcept;
        Builder& roughness(float roughness) noexcept;
        Builder& roughness(const std::shared_ptr<Texture>& texture) noexcept;
        Builder& ao(const std::shared_ptr<Texture>& texture) noexcept;
        Builder& orm(const std::shared_ptr<Texture>& texture) noexcept;
        Builder& ior(float ior) noexcept;
        Builder& absorption(float absorption) noexcept;
        Builder& microthickness(float microthickness) noexcept;
        Builder& thickness(float thickness) noexcept;
        Builder& reflectance(float reflectance) noexcept;
        Builder& transmission(float transmission) noexcept;

        Builder& normal_map() noexcept;
        Builder& orm_map() noexcept;

        Builder& blending(Blending blending) noexcept;
        Builder& shading(Shading shading) noexcept;
        Builder& two_sided(bool two_sided) noexcept;
        Builder& refraction(bool refraction) noexcept;
        Builder& name(const std::string& name) noexcept;

        Builder& vertex(const std::string& snippet) noexcept;
        Builder& fragment(const std::string& snippet) noexcept;
        Builder& global_fragment(const std::string& snippet) noexcept;
        Builder& global_vertex(const std::string& snippet) noexcept;
        Builder& shading(const std::string& snippet) noexcept;

        Builder& model(InstanceType model) noexcept;
        Builder& models(const InstanceTypes& models) noexcept;

        Builder& skybox() noexcept;

        Builder& time() noexcept;
        Builder& custom(const std::string& name, float value) noexcept;
        Builder& custom(const std::string& name, int32_t value) noexcept;
        Builder& custom(const std::string& name, uint32_t value) noexcept;
        Builder& custom(const std::string& name, const glm::vec2& value) noexcept;
        Builder& custom(const std::string& name, const glm::vec3& value) noexcept;
        Builder& custom(const std::string& name, const glm::vec4& value) noexcept;
        Builder& custom(const std::string& name, const glm::mat3& value) noexcept;
        Builder& custom(const std::string& name, const glm::mat4& value) noexcept;
        Builder& custom(const std::string& name, const std::shared_ptr<Texture>& value) noexcept;

        std::shared_ptr<Material> build(Assets& assets);
    };
}