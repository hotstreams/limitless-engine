#pragma once

#include <limitless/ms/material.hpp>
#include <limitless/pipeline/shader_pass_types.hpp>
#include <limitless/ms/unique_material.hpp>
#include <functional>
#include <mutex>
#include <limitless/pipeline/render_settings.hpp>

namespace Limitless {
    class Assets;
    class Context;

    namespace fx {
        class EffectBuilder;
    }
}

namespace Limitless::ms {
    struct material_builder_error : public std::runtime_error {
        explicit material_builder_error(const char* err) : std::runtime_error(err) {}
    };

    class MaterialBuilder {
    private:
        static inline std::map<UniqueMaterial, uint64_t> unique_materials;
        static inline uint64_t next_shader_index {};
        static inline std::mutex mutex;

        std::shared_ptr<Material> material;

        Context& context;
        Assets& assets;
        const RenderSettings& settings;

        [[nodiscard]] UniqueMaterial getMaterialType() const noexcept;
        void compileShaders(const ModelShaders&, const PassShaders&);
        void initializeMaterialBuffer();
        void checkRequirements();
        void setMaterialIndex();
        void setModelShaders();
        void setPassShaders();
        void createMaterial();
    public:
        MaterialBuilder(Context& context, Assets& assets, const RenderSettings& settings);
        virtual ~MaterialBuilder() = default;

        MaterialBuilder(const MaterialBuilder&) = delete;
        MaterialBuilder& operator=(const MaterialBuilder&) = delete;

        MaterialBuilder(MaterialBuilder&&) = delete;
        MaterialBuilder& operator=(MaterialBuilder&&) = delete;

        MaterialBuilder& add(Property type, std::shared_ptr<Texture> texture);
        MaterialBuilder& add(Property type, const glm::vec4& value);
        MaterialBuilder& add(Property type, const glm::vec2& value);
        MaterialBuilder& add(Property type, float value);
        MaterialBuilder& remove(Property type);

        MaterialBuilder& setBlending(Blending blending) noexcept;
        MaterialBuilder& setShading(Shading shading) noexcept;
        MaterialBuilder& setTwoSided(bool two_sided) noexcept;
        MaterialBuilder& setName(std::string name) noexcept;

        MaterialBuilder& setFragmentSnippet(std::string fs_code) noexcept;
        MaterialBuilder& setVertexSnippet(std::string vs_code) noexcept;
        MaterialBuilder& setTessellationSnippet(std::string tes_code) noexcept;
        MaterialBuilder& setGlobalSnippet(std::string global_code) noexcept;

        MaterialBuilder& addUniform(std::unique_ptr<Uniform> uniform);
        MaterialBuilder& removeUniform(const std::string& name);

        MaterialBuilder& setModelShaders(const ModelShaders& shaders) noexcept;
        MaterialBuilder& addPassShader(ShaderPass pass) noexcept;
        MaterialBuilder& addModelShader(ModelShader model) noexcept;

        MaterialBuilder& set(decltype(material->properties)&& properties);
        MaterialBuilder& set(decltype(material->uniforms)&& uniforms);

        std::shared_ptr<Material> build();
    };
}