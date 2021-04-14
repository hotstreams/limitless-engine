#pragma once

#include <limitless/material_system/material.hpp>
#include <limitless/shader_types.hpp>
#include <functional>
#include <mutex>

namespace LimitlessEngine {
    class Assets;
    class Context;

    struct material_builder_error : public std::runtime_error {
        explicit material_builder_error(const char* err) : std::runtime_error(err) {}
    };

    class MaterialBuilder {
    private:
        static inline std::map<MaterialType, uint64_t> unique_materials;
        [[nodiscard]] MaterialType getMaterialType() const noexcept;
    protected:
        static inline uint64_t next_shader_index {};
        static inline std::mutex mutex;

        std::shared_ptr<Material> material;

        Context& context;
        Assets& assets;

        void compileShaders(const ModelShaders& model_shaders, const MaterialShaders& material_shaders);
        static void initializeMaterialBuffer(Material& mat, const ShaderProgram& shader) noexcept;
        void setProperties(decltype(Material::properties)&& props);
        virtual void setMaterialIndex();
        virtual void checkRequirements();

        friend class EffectBuilder;
        friend class MaterialSerializer;
    public:
        MaterialBuilder(Context& context, Assets& _ctx) noexcept;
        virtual ~MaterialBuilder() = default;

        MaterialBuilder(const MaterialBuilder&) = delete;
        MaterialBuilder& operator=(const MaterialBuilder&) = delete;

        MaterialBuilder(MaterialBuilder&&) = delete;
        MaterialBuilder& operator=(MaterialBuilder&&) = delete;

        MaterialBuilder& add(PropertyType type, std::shared_ptr<Texture> texture);
        MaterialBuilder& add(PropertyType type, const glm::vec4& value);
        MaterialBuilder& add(PropertyType type, float value);
        MaterialBuilder& remove(PropertyType type);

        MaterialBuilder& setBlending(Blending blending) noexcept;
        MaterialBuilder& setShading(Shading shading) noexcept;
        MaterialBuilder& setTwoSided(bool two_sided) noexcept;

        // creates Material
        virtual MaterialBuilder& create(std::string name);

        // builds shaders
        std::shared_ptr<Material> build(const ModelShaders& model_shaders = {ModelShader::Model},
                                        const MaterialShaders& material_shaders = {MaterialShader::Forward, MaterialShader::DirectionalShadow});
    };
}