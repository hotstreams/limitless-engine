#pragma once

#include <limitless/core/shader_compiler.hpp>

namespace Limitless {
    enum class ShaderPass;
    enum class ModelShader;
    class Assets;
    class RenderSettings;
}

namespace Limitless::ms {
    class Material;

	class material_compilation_error : public std::runtime_error {
	public:
	    using std::runtime_error::runtime_error;
	};

    class MaterialCompiler : public ShaderCompiler {
    protected:
        Assets& assets;

        static std::string getCustomMaterialScalarUniforms(const Material& material) noexcept;
        static std::string getCustomMaterialSamplerUniforms(const Material& material) noexcept;
        std::string getMaterialDefines(const Material& material) noexcept;
        static std::string getModelDefines(const ModelShader& type);

        void replaceMaterialSettings(Shader& shader, const Material& material, ModelShader model_shader) noexcept;
    public:
        MaterialCompiler(Context& context, Assets& assets, const RenderSettings& settings) noexcept;
        ~MaterialCompiler() override = default;

        using ShaderCompiler::compile;
        void compile(const Material& material, ShaderPass pass_shader, ModelShader model_shader);
    };
}
