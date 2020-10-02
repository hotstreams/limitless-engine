#pragma once

#include <shader_compiler.hpp>

namespace GraphicsEngine {
    enum class ModelShader;
    enum class MaterialShader;
    struct MaterialType;
    class MaterialBuilder;
    class CustomMaterialBuilder;
    class SpriteEmitter;

    class MaterialCompiler : public ShaderCompiler {
    private:
        std::string getMaterialDefines(const MaterialType& type) noexcept;
        std::string getModelDefines(const ModelShader& type) noexcept;
        std::string getEmitterDefines(const SpriteEmitter& emitter) noexcept;
        std::string getCustomMaterialScalarUniforms(const CustomMaterialBuilder& builder) noexcept;
        std::string getCustomMaterialSamplerUniforms(const CustomMaterialBuilder& builder) noexcept;

        static void replaceSettings(Shader& src) noexcept;
    public:
        using ShaderCompiler::compile;

        void compile(const MaterialBuilder& builder, MaterialShader material_type, ModelShader model_type);
        void compile(const CustomMaterialBuilder& builder, MaterialShader material_type, ModelShader model_type);
        void compile(const SpriteEmitter& emitter);
    };
}
