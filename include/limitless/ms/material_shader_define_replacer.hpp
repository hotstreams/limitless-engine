#pragma once

#include <string>
#include <unordered_map>
#include <limitless/ms/property.hpp>
#include <limitless/ms/shading.hpp>
#include <limitless/pipeline/shader_type.hpp>
#include <limitless/core/shader/shader_define_replacer.hpp>

namespace Limitless {
    class Shader;

    namespace ms {
        class Material;

        class MaterialShaderDefineReplacer : public ShaderDefineReplacer {
        protected:
            static inline std::unordered_map<Property, std::string> PROPERTY_DEFINE =
            {
                { Property::Color, "ENGINE_MATERIAL_COLOR" },
                { Property::EmissiveColor, "ENGINE_MATERIAL_EMISSIVE_COLOR" },
                { Property::Diffuse, "ENGINE_MATERIAL_DIFFUSE_TEXTURE" },
                { Property::Normal, "ENGINE_MATERIAL_NORMAL_TEXTURE" },
                { Property::EmissiveMask, "ENGINE_MATERIAL_EMISSIVEMASK_TEXTURE" },
                { Property::BlendMask, "ENGINE_MATERIAL_BLENDMASK_TEXTURE" },
                { Property::MetallicTexture, "ENGINE_MATERIAL_METALLIC_TEXTURE" },
                { Property::RoughnessTexture, "ENGINE_MATERIAL_ROUGHNESS_TEXTURE" },
                { Property::AmbientOcclusionTexture, "ENGINE_MATERIAL_AMBIENT_OCCLUSION_TEXTURE" },
                { Property::ORM, "ENGINE_MATERIAL_ORM_TEXTURE" },
                { Property::Metallic, "ENGINE_MATERIAL_METALLIC" },
                { Property::Roughness, "ENGINE_MATERIAL_ROUGHNESS" },
                { Property::IoR, "ENGINE_MATERIAL_IOR" },
                { Property::Absorption, "ENGINE_MATERIAL_ABSORPTION" },
                { Property::MicroThickness, "ENGINE_MATERIAL_MICROTHICKNESS" },
                { Property::Thickness, "ENGINE_MATERIAL_THICKNESS" },
                { Property::Transmission, "ENGINE_MATERIAL_TRANSMISSION" },
                { Property::Reflectance, "ENGINE_MATERIAL_REFLECTANCE" },
            };

            static inline std::unordered_map<Shading, std::string> SHADING_DEFINE =
            {
                { Shading::Lit, "ENGINE_MATERIAL_SHADING_REGULAR_MODEL" },
                { Shading::Unlit, "ENGINE_MATERIAL_SHADING_UNLIT_MODEL" },
                { Shading::Custom, "ENGINE_MATERIAL_SHADING_CUSTOM_MODEL" },
            };

            static inline std::unordered_map<InstanceType, std::string> MODEL_DEFINE =
            {
                { InstanceType::Model, "ENGINE_MATERIAL_REGULAR_MODEL" },
                { InstanceType::Skeletal, "ENGINE_MATERIAL_SKELETAL_MODEL" },
                { InstanceType::Instanced, "ENGINE_MATERIAL_INSTANCED_MODEL" },
                { InstanceType::SkeletalInstanced, "ENGINE_MATERIAL_SKELETAL_INSTANCED_MODEL" },
                { InstanceType::Effect, "ENGINE_MATERIAL_EFFECT_MODEL" },
                { InstanceType::Decal, "ENGINE_MATERIAL_DECAL_MODEL" },
            };

            enum class SnippetDefineType {
                Vertex,
                Fragment,
                Global,
                CustomScalar,
                CustomSamplers,
                CustomShading
            };

            static inline std::unordered_map<SnippetDefineType, std::string> SNIPPET_DEFINE =
            {
                { SnippetDefineType::Vertex, "ENGINE_MATERIAL_VERTEX_SNIPPET" },
                { SnippetDefineType::Fragment, "ENGINE_MATERIAL_FRAGMENT_SNIPPET" },
                { SnippetDefineType::Global, "ENGINE_MATERIAL_GLOBAL_DEFINITIONS" },
                { SnippetDefineType::CustomScalar, "ENGINE_MATERIAL_CUSTOM_SCALARS" },
                { SnippetDefineType::CustomSamplers, "ENGINE_MATERIAL_CUSTOM_SAMPLERS" },
                { SnippetDefineType::CustomShading, "ENGINE_MATERIAL_SHADING_CUSTOM_SNIPPET" },
            };

            static std::string getPropertyDefines(const Material& material);
            static std::string getShadingDefines(const Material& material);
            static std::string getModelDefines(InstanceType model_shader);
            static std::string getMaterialDefines(const Material& material);
            static std::string getMaterialDependentDefine(const Material& material, InstanceType model_shader);
            static std::string getScalarUniformDefines(const Material& material);
            static std::string getSamplerUniformDefines(const Material& material);

            MaterialShaderDefineReplacer() noexcept = default;
        public:
            static void replaceMaterialDependentDefine(Shader& shader, const Material& material, InstanceType model_shader);
        };
    }
}