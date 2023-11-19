#pragma once

#include <limitless/core/shader/shader_compiler.hpp>

namespace Limitless {
    enum class ShaderType;
    enum class InstanceType;
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

        static void replaceMaterialSettings(Shader& shader, const Material& material, InstanceType model_shader) noexcept;
    public:
        MaterialCompiler(Context& context, Assets& assets, const RenderSettings& settings) noexcept;
        ~MaterialCompiler() override = default;

        using ShaderCompiler::compile;
        void compile(const Material& material, ShaderType pass_shader, InstanceType model_shader);
    };
}
