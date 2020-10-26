#pragma once

#include <core/shader_compiler.hpp>

namespace GraphicsEngine {
    enum class MaterialShader;
    enum class ModelShader;
    class CustomMaterial;
    class Material;

    class MaterialCompiler : public ShaderCompiler {
    protected:
        std::string getCustomMaterialScalarUniforms(const CustomMaterial& material) noexcept;
        std::string getCustomMaterialSamplerUniforms(const CustomMaterial& material) noexcept;
        std::string getMaterialDefines(const Material& material) noexcept;
        std::string getModelDefines(const ModelShader& type) noexcept;

        void replaceMaterialSettings(Shader& shader, const Material& material, ModelShader model_shader) noexcept;
        void replaceRenderSettings(Shader& src) noexcept;
    public:
        using ShaderCompiler::compile;
        void compile(const Material& material, MaterialShader material_shader, ModelShader model_shader);
    };
}
