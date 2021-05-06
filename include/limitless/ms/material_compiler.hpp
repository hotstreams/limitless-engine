#pragma once

#include <limitless/core/shader_compiler.hpp>

namespace Limitless {
    enum class ShaderPass;
    enum class ModelShader;
    class Assets;
}

namespace Limitless::ms {
    class Material;

    class MaterialCompiler : public ShaderCompiler {
    protected:
        Assets& assets;

        static std::string getCustomMaterialScalarUniforms(const Material& material) noexcept;
        static std::string getCustomMaterialSamplerUniforms(const Material& material) noexcept;
        static std::string getMaterialDefines(const Material& material) noexcept;
        static std::string getModelDefines(const ModelShader& type);

        static void replaceMaterialSettings(Shader& shader, const Material& material, ModelShader model_shader) noexcept;
        static void replaceRenderSettings(Shader& src) noexcept;
    public:
        MaterialCompiler(Context& context, Assets& assets) noexcept;
        ~MaterialCompiler() override = default;

        using ShaderCompiler::compile;
        void compile(const Material& material, ShaderPass material_shader, ModelShader model_shader);
    };
}
