#pragma once

#include <string>
#include <unordered_map>
#include <limitless/ms/property.hpp>
#include <limitless/ms/shading.hpp>
#include <limitless/renderer/shader_type.hpp>
#include <limitless/core/shader/shader_define_replacer.hpp>
#include <limitless/core/vertex_stream/vertex_stream.hpp>

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
                { InstanceType::Terrain, "ENGINE_MATERIAL_TERRAIN_MODEL" },
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

            enum class VertexDefineType {
                Stream,
                Context,
                ContextAssignment,
                PassThrough
            };

            static inline std::unordered_map<VertexDefineType, std::string> VERTEX_STREAM_DEFINE =
            {
                { VertexDefineType::Stream, "ENGINE::VERTEX_STREAM" },
                { VertexDefineType::Context, "ENGINE::VERTEX_CONTEXT" },
                { VertexDefineType::ContextAssignment, "ENGINE::ASSIGN_STREAM_ATTIRIBUTES" },
                { VertexDefineType::PassThrough, "ENGINE::PASS_THROUGH" }
            };

            static inline std::string vertex_input_prefix = "_vertex_";

            static std::string getPropertyDefines(const Material& material);
            static std::string getShadingDefines(const Material& material);
            static std::string getModelDefines(InstanceType model_shader);
            static std::string getMaterialDefines(const Material& material);
            static std::string getMaterialDependentDefine(const Material& material, InstanceType model_shader);
            static std::string getScalarUniformDefines(const Material& material);
            static std::string getSamplerUniformDefines(const Material& material);


            static std::string getMaterialBufferDeclaration(const Material& material);
            static std::string getMaterialGettersDeclaration(const Material& material);


            /*
             *  Vertex Input Stream
             */
            static std::string getVertexStreamDeclaration(InstanceType instance_type);
            static std::string getVertexStreamGettersDeclaration(InstanceType instance_type);

            static std::string getVertexContextDeclaration(
                const Material& material,
                const RendererSettings& settings,
                InstanceType type
            );

            static std::string getVertexContextCompute(
                const Material& material,
                const RendererSettings& settings,
                InstanceType type
            );

            static std::string getVertexContextInterfaceBlock(
                const Material& material,
                const RendererSettings& settings,
                InstanceType type
            );

            static std::string getVertexContextInterfaceBlockOut(
                const Material& material,
                const RendererSettings& settings,
                InstanceType type
            );

            static std::string getVertexContextInterfaceBlockIn(
                const Material& material,
                const RendererSettings& settings,
                InstanceType type
            );

            static std::string getVertexPassThrough(
                const Material& material,
                const RendererSettings& settings,
                InstanceType type
            );

            MaterialShaderDefineReplacer() noexcept = default;
        public:
            static void replaceMaterialDependentDefine(
                Shader& shader,
                const Material& material,
                const RendererSettings& settings,
                InstanceType model_shader);
        };
    }
}
